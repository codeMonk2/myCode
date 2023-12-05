package main

import (
    "fmt"
    "flag"
    "io/ioutil"
    "strconv"
    "strings"
    "sync"
    "time"
)

type Node struct {
   val   int
   left  *Node
   right *Node
}

type bst struct {
    root *Node
}

type concurrentContainer struct {
    lock sync.Mutex
    // Some data is available to be processed by the workers
    hasData *sync.Cond
    // Main thread has slots available to push data
    hasSlot *sync.Cond
    data []struct{a,b int}
}

type options struct {
    numTrees int
    hashw int
    dataw int
    compw int
    compwFlag int
    inFile *string
    trees []*bst
    hashToBstIdMap map[int][]int
    lock sync.Mutex
    adjMatrix [][]bool
    buf concurrentContainer
}

func timeTrack(start time.Time, name string) {
    elapsed := time.Since(start)
    fmt.Printf("%s %f\n", name, elapsed.Seconds())
}

func (t *bst) insert (data int) {
    if t.root == nil {
        t.root = &Node{val: data, left: nil, right: nil}
    } else {
        t.root.insert(data)
    }
}

func (n* Node) insert (data int) {
    if (data < n.val) {
        if n.left == nil {
            n.left = &Node{val: data, left: nil, right: nil}
        } else {
            n.left.insert(data)
        }
    } else {
        if n.right == nil {
            n.right = &Node{val: data, left: nil, right: nil}
        } else {
            n.right.insert(data)
        }
    }
}

func printInOrder(n *Node) {
    if n == nil {
        return
    } else {
        printInOrder(n.left)
        fmt.Printf("%d ", n.val)
        printInOrder(n.right)
    }
}

func printHashToBstIdMap(opts *options) {
    for k, bstIds := range(opts.hashToBstIdMap) {
        if len(bstIds) == 1 {
            continue
        }
        fmt.Printf("%d: ", k)
        for _, v := range(bstIds) {
            fmt.Printf("%02d ", v)
        }
        fmt.Printf("\n")
    }
}

func printAdjMatrixGroups(opts *options) {
    a := make([]int, opts.numTrees)
    for i := 0; i < opts.numTrees; i++ {
        a[i] = 1
    }
    groupCnt := 0
    for i := 0; i < opts.numTrees; i++ {
        if a[i] != 1 {
            continue
        }
        matchCnt := 0
        for j := 0; j < opts.numTrees; j++ {
            if opts.adjMatrix[i][j] == true {
                matchCnt++
            }
        }
        if matchCnt == 1 {
            a[i] = 0
            continue
        }
        fmt.Printf("group %d: ", groupCnt);
        for j := 0; j < opts.numTrees; j++ {
            if opts.adjMatrix[i][j] == true {
                fmt.Printf("%02d ", j)
                a[j] = 0
            }
        }
        a[i] = 0
        fmt.Printf("\n")
        groupCnt++
    }
}

func calculateHash(hash int, value int) int {
    new_value := value + 2
    hash = (hash * new_value + new_value) % 1000
    return hash
}

func inOrderTraverseAndCalculateHash(n *Node, hash *int) {
    if n == nil {
        return
    }

    inOrderTraverseAndCalculateHash(n.left, hash)
    *hash = calculateHash(*hash, n.val)
    inOrderTraverseAndCalculateHash(n.right, hash)
}

func inOrderTraverseSeq(n *Node, a *[]int) {
    if n == nil {
        return
    } else {
        inOrderTraverseSeq(n.left, a)
        *a = append(*a, n.val)
        inOrderTraverseSeq(n.right, a)
    }
}

func areTreesEqSeq(t1, t2 *bst) bool {
    var a []int
    var b []int
    inOrderTraverseSeq(t1.root, &a)
    inOrderTraverseSeq(t2.root, &b)

    if len(a) != len(b) {
        return false
    }

    for i,v := range(a) {
        if v != b[i] {
            return false
        }
    }
    return true
}

func inOrderTraverseParallel(n *Node, ch chan int) {
    if n == nil {
        return
    }
    inOrderTraverseParallel(n.left, ch)
    ch <- n.val
    inOrderTraverseParallel(n.right, ch)
}

func areTreesEqParallel(t1, t2 *bst) bool {
    c1, c2 := make(chan int), make(chan int)
    go func() {
        defer close(c1)
        inOrderTraverseParallel(t1.root, c1)
    }()
    go func() {
        defer close(c2)
        inOrderTraverseParallel(t2.root, c2)
    }()
    for {
        val1, ok1 := <- c1
        val2, ok2 := <- c2
        if ok1 != ok2 || val1 != val2 {
            return false
        }
        if !ok1 && !ok2 {
            break
        }
    }
    return true
}

func computeHashParallelAll(opts *options, idx int, wg * sync.WaitGroup) {
    defer wg.Done()
    var hash = 1
    inOrderTraverseAndCalculateHash(opts.trees[idx].root, &hash)
}

// If hash_workers = 1 and everything else is zero
func computeHashSeq(opts *options) {
    defer timeTrack(time.Now(), "hashTime:")
    for i := 0; i < opts.numTrees; i++ {
        var hash = 1
        inOrderTraverseAndCalculateHash(opts.trees[i].root, &hash)
        //fmt.Printf("id %d hash %d\n", i, hash)
    }
}

// If hash_workers > 1 and everything else is zero
func computeHashParallel(opts *options, idx int, wg * sync.WaitGroup) {
    defer wg.Done()
    for i := idx; i < opts.numTrees; i += opts.hashw  {
        var hash = 1
        inOrderTraverseAndCalculateHash(opts.trees[i].root, &hash)
        //fmt.Printf("id %d hash %d\n", i, hash)
    }
}

// If hash_workers = 1, data_workers = 1
func computeHashSeqWithMapUpdate(opts *options) {
    for i := 0; i < opts.numTrees; i++ {
        var hash = 1
        inOrderTraverseAndCalculateHash(opts.trees[i].root, &hash)
        opts.hashToBstIdMap[hash] = append(opts.hashToBstIdMap[hash], i)
    }
}

func computeHashAndReturnInChannel(opts *options, idx int, wg * sync.WaitGroup, queue chan struct {k,v int}) {
    defer wg.Done()
    for i := idx; i < opts.numTrees; i += opts.hashw  {
        var hash = 1
        inOrderTraverseAndCalculateHash(opts.trees[i].root, &hash)
        queue <- struct{k,v int}{i,hash}
    }
}

// If hash_workers > 1, data_workers = hash_workers
func computeHashParallelWithMapUpdate(opts *options, idx int, wg *sync.WaitGroup) {
    defer wg.Done()
    var wg1 sync.WaitGroup
    c := make(chan struct{k,v int})
    for i := idx; i < opts.hashw; i += opts.dataw {
        wg1.Add(1)
        go computeHashAndReturnInChannel(opts, i, &wg1, c)
    }
    go func() {
        wg1.Wait()
        close(c)
    }()

    if opts.dataw > 1 {
        opts.lock.Lock()
        defer opts.lock.Unlock()
    }
    for pair := range c {
        opts.hashToBstIdMap[pair.v] = append(opts.hashToBstIdMap[pair.v], pair.k)
    }
}

func compareTreesParallel(i1,i2 int, opts *options,  wg * sync.WaitGroup) {
    defer wg.Done()
    opts.adjMatrix[i1][i2] = areTreesEqParallel(opts.trees[i1], opts.trees[i2])
    opts.adjMatrix[i2][i1] = opts.adjMatrix[i1][i2]
}

func pop(data []struct{a,b int}) []struct{a,b int} {
    return append(data[:0], data[1:]...)
}

func computeAdj(id int, opts *options, wg * sync.WaitGroup) {
    defer wg.Done()
    for {
        opts.buf.lock.Lock()

        for len(opts.buf.data) == 0 {
            opts.buf.hasData.Wait()
        }

        data := opts.buf.data[0]

        if data.a == -1 && data.b == -1 {
            opts.buf.lock.Unlock()
            break;
        }

        opts.buf.data = pop(opts.buf.data)

        //signal slot is available to producer
        opts.buf.hasSlot.Signal()

        opts.adjMatrix[data.a][data.b] = areTreesEqParallel(opts.trees[data.a], opts.trees[data.b])
        opts.adjMatrix[data.b][data.a] = opts.adjMatrix[data.a][data.b]

        opts.buf.lock.Unlock()
    }
}

func computeAdjMatrix(opts *options) {
    for _, bstIds := range(opts.hashToBstIdMap) {
        if len(bstIds) == 1 {
            opts.adjMatrix[bstIds[0]][bstIds[0]] = true
        } else {
            for idx, i1 := range(bstIds) {
                for _, i2 := range(bstIds[idx+1:]) {
                    if i1 != i2 {
                        opts.buf.lock.Lock()
                        if len(opts.buf.data) == opts.compw {
                            opts.buf.hasSlot.Wait()
                        }
                        opts.buf.data = append(opts.buf.data, struct{a,b int}{i1, i2})
                        opts.buf.hasData.Signal()
                        opts.buf.lock.Unlock()
                    }
                }
            }
        }
    }
    opts.buf.lock.Lock()
    for len(opts.buf.data) != 0 {
        opts.buf.hasSlot.Wait()
    }
    opts.buf.data = append(opts.buf.data, struct{a,b int}{-1,-1})
    opts.buf.hasData.Broadcast()
    opts.buf.lock.Unlock()
}

func readInput(opts *options) {
    content, err := ioutil.ReadFile(*opts.inFile)
    if err != nil {
        fmt.Printf("Error\n")
        return
    }
    lines := strings.Split(string(content), "\n")
    var i = 0
    for i < len(lines) {
        t := new(bst)
        for _, val := range strings.Split(lines[i], " ") {
            v, _ := strconv.Atoi(val)
            t.insert(v)
        }
        opts.trees = append(opts.trees, t)
        i++
    }
    opts.numTrees = i-1
}

func argParse(opts *options) {
    hashwP     := flag.Int("hash-workers", 1, "number of workers to calculate hash")
    datawP     := flag.Int("data-workers", 0, "number of workers who can update the hash map")
    compwP     := flag.Int("comp-workers", 0, "number of workers who can compare hashes")
    compwFlagP := flag.Int("comp-workers-flag", 0, "flag for compare workers implementation selection 0/1")
    opts.inFile = flag.String("input", "", "Input file")

    flag.Parse()

    opts.hashw = *hashwP
    opts.dataw = *datawP
    opts.compw = *compwP
    opts.compwFlag = *compwFlagP

}

func main() {

    var opts options
    var wg sync.WaitGroup

    argParse(&opts)

    if *opts.inFile == "" {
        fmt.Printf("Invalid input file\n")
        return
    }

    if opts.dataw > opts.hashw {
        fmt.Printf("Invalid input - data-workers > hash-workers\n")
        return
    }

    if opts.compwFlag != 0 && opts.compwFlag != 1 {
        fmt.Printf("Invalid input - compwFlag %d. Only 0/1 allowed\n", opts.compwFlag)
        return
    }

    readInput(&opts)

    // If hash_workers = 1 run sequential
    if opts.hashw == 1 && opts.dataw == 0 && opts.compw == 0 {
        //computeHashSeq(&opts)
        computeHashSeq(&opts)
    } else if opts.hashw > 1 && opts.dataw == 0 && opts.compw == 0 {
        // If hash_workers > 1 and everything else is 0 just calculate hash
        start := time.Now()
        var test = 1
        if test == 0 {
            for i := 0; i < opts.numTrees; i++ {
                wg.Add(1)
                go computeHashParallelAll(&opts, i, &wg)
            }
        } else {
            for i := 0; i < opts.hashw; i++ {
                wg.Add(1)
                go computeHashParallel(&opts, i, &wg)
            }
        }
        wg.Wait()
        elapsed := time.Since(start)
        fmt.Printf("hashTime: %f\n", elapsed.Seconds())
    } else if opts.hashw == 1 && opts.dataw == 1 {
        opts.hashToBstIdMap = make(map[int][]int)
        start := time.Now()
        computeHashSeqWithMapUpdate(&opts)
        elapsed := time.Since(start)
        fmt.Printf("hashGroupTime: %f\n", elapsed.Seconds())
        printHashToBstIdMap(&opts)
    } else if opts.hashw > 1 && opts.dataw <= opts.hashw {
        opts.lock = sync.Mutex{}
        opts.hashToBstIdMap = make(map[int][]int)
        start := time.Now()
        for i := 0; i < opts.dataw; i++ {
            wg.Add(1)
            go computeHashParallelWithMapUpdate(&opts, i, &wg)
        }
        wg.Wait()
        elapsed := time.Since(start)
        fmt.Printf("hashGroupTime: %f\n", elapsed.Seconds())
        printHashToBstIdMap(&opts)
    }
    // hashw = dataw = 1 compw = 1 or > 1
    // hashw = dataw > 1 compw = 1 or > 1
    if (opts.compw > 0) {
        opts.adjMatrix = make([][]bool, opts.numTrees)

        for i := range(opts.adjMatrix) {
            opts.adjMatrix[i] = make([]bool, opts.numTrees)
            for j := range(opts.adjMatrix[i]) {
                if i == j {
                    opts.adjMatrix[i][j] = true;
                } else {
                    opts.adjMatrix[i][j] = false;
                }
            }
        }

        if (opts.compw == 1) {
            start := time.Now()
            for _, bstIds := range(opts.hashToBstIdMap) {
                if len(bstIds) == 1 {
                    opts.adjMatrix[bstIds[0]][bstIds[0]] = true
                } else {
                    for idx, i1 := range(bstIds) {
                        for _, i2 := range(bstIds[idx+1:]) {
                            if i1 != i2 {
                                opts.adjMatrix[i1][i2] = areTreesEqSeq(opts.trees[i1], opts.trees[i2])
                                opts.adjMatrix[i2][i1] = opts.adjMatrix[i1][i2]
                            }
                        }
                    }
                }
            }
            elapsed := time.Since(start)
            fmt.Printf("compareTreeTime: %f\n", elapsed.Seconds())
            printAdjMatrixGroups(&opts)
        } else {
            if opts.compwFlag == 0 {
                start := time.Now()
                for _, bstIds := range(opts.hashToBstIdMap) {
                    if len(bstIds) == 1 {
                        opts.adjMatrix[bstIds[0]][bstIds[0]] = true
                    } else {
                        for idx, i1 := range(bstIds) {
                            for _, i2 := range(bstIds[idx+1:]) {
                                if i1 != i2 {
                                    wg.Add(1)
                                    go compareTreesParallel(i1, i2, &opts, &wg)
                                }
                            }
                        }
                    }
                }
                wg.Wait()
                elapsed := time.Since(start)
                fmt.Printf("compareTreeTime: %f\n", elapsed.Seconds())
                printAdjMatrixGroups(&opts)
            } else {
                opts.buf.lock = sync.Mutex{}
                opts.buf.hasData = sync.NewCond(&opts.buf.lock)
                opts.buf.hasSlot = sync.NewCond(&opts.buf.lock)
                for i := 0; i < opts.compw; i++ {
                    wg.Add(1)
                    go computeAdj(i, &opts, &wg)
                }
                start := time.Now()
                computeAdjMatrix(&opts)
                wg.Wait()
                elapsed := time.Since(start)
                fmt.Printf("compareTreeTime: %f\n", elapsed.Seconds())
                printAdjMatrixGroups(&opts)
            }
        }
    }

    return
}
