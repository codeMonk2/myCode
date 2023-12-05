//!
//! coordinator.rs
//! Implementation of 2PC coordinator
//!
extern crate log;
extern crate stderrlog;
extern crate rand;
extern crate ipc_channel;
extern crate queues;

use std::sync::Arc;
//use std::sync::Mutex;
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;
use std::time::Duration;
use std::collections::HashMap;

use coordinator::ipc_channel::ipc::IpcSender as Sender;
use coordinator::ipc_channel::ipc::IpcReceiver as Receiver;
use coordinator::ipc_channel::ipc::TryRecvError;
use coordinator::queues::Queue;
use coordinator::queues::IsQueue;

use message;
use message::MessageType;
use message::ProtocolMessage;
use oplog;

/// Connection Data
#[derive(Debug)]
pub struct ConnData {
    tx : Sender<ProtocolMessage>,
    rx : Receiver<ProtocolMessage>,
}

/// CoordinatorState
/// States for 2PC state machine
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum CoordinatorState {
    Quiescent,
    ReceivedRequest,
    ProposalSent,
    ReceivedVotesAbort,
    ReceivedVotesCommit,
    SentGlobalDecision
}

/// Coordinator
/// Struct maintaining state for coordinator
#[derive(Debug)]
pub struct Coordinator {
    state: CoordinatorState,
    running: Arc<AtomicBool>,
    log: oplog::OpLog,
    msg_q : Queue<ProtocolMessage>,
    client_conn_map : HashMap<u32, ConnData>,
    participant_conn_map : HashMap<u32, ConnData>,
    commit_ops : u64,
    abort_ops : u64,
    unknown_ops : u64,
}

///
/// Coordinator
/// Implementation of coordinator functionality
/// Required:
/// 1. new -- Constructor
/// 2. protocol -- Implementation of coordinator side of protocol
/// 3. report_status -- Report of aggregate commit/abort/unknown stats on exit.
/// 4. participant_join -- What to do when a participant joins
/// 5. client_join -- What to do when a client joins
///
impl Coordinator {

    ///
    /// new()
    /// Initialize a new coordinator
    ///
    /// <params>
    ///     log_path: directory for log files --> create a new log there.
    ///     r: atomic bool --> still running?
    ///
    pub fn new(
        log_path: String,
        r: Arc<AtomicBool>) -> Coordinator {

        Coordinator {
            state: CoordinatorState::Quiescent,
            log: oplog::OpLog::new(log_path),
            running: r,
            msg_q : Queue::<ProtocolMessage>::new(),
            client_conn_map : HashMap::new(),
            participant_conn_map : HashMap::new(),
            commit_ops : 0,
            abort_ops : 0,
            unknown_ops : 0,
        }
    }

    ///
    /// participant_join()
    /// Adds a new participant for the coordinator to keep track of
    ///
    /// HINT: Keep track of any channels involved!
    /// HINT: You may need to change the signature of this function
    ///
    pub fn participant_join(&mut self, participant_id : u32, t : Sender<ProtocolMessage>, r : Receiver<ProtocolMessage>) {
        assert!(self.state == CoordinatorState::Quiescent);

        let c1 = ConnData {tx : t, rx : r};
        self.participant_conn_map.insert(participant_id, c1);
    }

    ///
    /// client_join()
    /// Adds a new client for the coordinator to keep track of
    ///
    /// HINT: Keep track of any channels involved!
    /// HINT: You may need to change the signature of this function
    ///
    pub fn client_join(&mut self, client_id : u32, t : Sender<ProtocolMessage>, r : Receiver<ProtocolMessage>) {
        assert!(self.state == CoordinatorState::Quiescent);

        let c1 = ConnData {tx : t, rx : r};
        self.client_conn_map.insert(client_id, c1);
    }

    ///
    /// report_status()
    /// Report the abort/commit/unknown status (aggregate) of all transaction
    /// requests made by this coordinator before exiting.
    ///
    pub fn report_status(&mut self) {
        let successful_ops: u64 = self.commit_ops;
        let failed_ops: u64 = self.abort_ops;
        let unknown_ops: u64 = self.unknown_ops;

        println!("coordinator     :\tCommitted: {:6}\tAborted: {:6}\tUnknown: {:6}", successful_ops, failed_ops, unknown_ops);
    }

    pub fn client_recv(&mut self) {
        for (_, x) in self.client_conn_map.iter() {
            if self.running.load(Ordering::SeqCst) != true {
                return;
            }
            match x.rx.try_recv() {
                Ok(msg) => {
                    let _ = self.msg_q.add(msg.clone());
                },
                Err(e) => {
                    match e {
                        TryRecvError::Empty => {
                        }
                        _ => {
                            break;
                        }
                    }
                }
            }
        }
    }

    pub fn handle_messages(&mut self) {
        let m = self.msg_q.remove().ok();
        if let Some(mut msg) = m {
            if msg.mtype == MessageType::CoordinatorExit {
                self.client_conn_map.remove(&msg.senderid.parse::<u32>().unwrap());
                return;
            }
            self.state = CoordinatorState::ReceivedRequest;
            for (_, x) in self.participant_conn_map.iter() {
                msg.mtype = MessageType::CoordinatorPropose;
                if self.running.load(Ordering::SeqCst) != true {
                    return;
                }
                x.tx.send(msg.clone()).unwrap();
            }
            thread::sleep(Duration::from_millis(100));
            let mut num_aborts:u64 = 0;
            let mut num_commits:u64 = 0;
            for (_, x) in self.participant_conn_map.iter() {
                if self.running.load(Ordering::SeqCst) != true {
                    return;
                }
                match x.rx.try_recv() {
                    Ok(msg) => {
                        match msg.mtype {
                            MessageType::ParticipantVoteCommit => {
                                num_commits += 1;
                            },
                            MessageType::ParticipantVoteAbort => {
                                num_aborts += 1;
                            },
                            _ => {}
                        }
                    },
                    Err(e) => {
                        match e {
                            TryRecvError::Empty => {
                            }
                            _ => {
                                break;
                            }
                        }
                    }
                }
            }
            if (num_commits+num_aborts) != self.participant_conn_map.len() as u64 { 
                msg.mtype = MessageType::CoordinatorAbort;
                self.log.append(msg.mtype, msg.txid.clone(), msg.senderid.clone(), msg.opid);
                self.abort_ops += 1;
                //self.unknown_ops += 1;
            } else {
                let mut t:MessageType = MessageType::CoordinatorCommit;
                if num_aborts > 0 {
                    self.abort_ops += 1;
                    t = MessageType::CoordinatorAbort;
                } else {
                    self.commit_ops += 1;
                }
                msg.mtype = t;
                self.log.append(msg.mtype, msg.txid.clone(), msg.senderid.clone(), msg.opid);
            }
            for (_, x) in self.participant_conn_map.iter() {
                if self.running.load(Ordering::SeqCst) != true {
                    return;
                }
                x.tx.send(msg.clone()).unwrap();
            }
            {
                let mut t = MessageType::ClientResultCommit;
                if msg.mtype == MessageType::CoordinatorAbort {
                    t = MessageType::ClientResultAbort;
                }
                let x: &mut ConnData = self.client_conn_map.get_mut(&msg.senderid.parse::<u32>().unwrap()).unwrap();
                msg.mtype = t;
                if self.running.load(Ordering::SeqCst) != true {
                    return;
                }
                x.tx.send(msg.clone()).unwrap();
            }
        }
    }

    ///
    /// protocol()
    /// Implements the coordinator side of the 2PC protocol
    /// HINT: If the simulation ends early, don't keep handling requests!
    /// HINT: Wait for some kind of exit signal before returning from the protocol!
    ///
    pub fn protocol(&mut self) {
        loop {
            self.client_recv();
            if self.msg_q.size() != 0 {
                self.handle_messages();
            } else {
                thread::sleep(Duration::from_millis(100));
            }
            if self.running.load(Ordering::SeqCst) != true {
                break;
            }
            if self.client_conn_map.len() == 0 {
                self.state = CoordinatorState::ReceivedRequest;
                let pm = message::ProtocolMessage::generate(MessageType::CoordinatorExit,
                                                            "0".to_string(), "0".to_string(), 0);
                for (_, x) in self.participant_conn_map.iter() {
                    if self.running.load(Ordering::SeqCst) != true {
                        break;
                    }
                    x.tx.send(pm.clone()).unwrap();
                }
                thread::sleep(Duration::from_millis(100));
                break;
            }
        }
        self.report_status();
    }
}
