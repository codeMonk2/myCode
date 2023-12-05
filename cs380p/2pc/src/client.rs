//!
//! client.rs
//! Implementation of 2PC client
//!
extern crate ipc_channel;
extern crate log;
extern crate stderrlog;

use std::thread;
use std::time::Duration;
use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};

use client::ipc_channel::ipc::IpcReceiver as Receiver;
use client::ipc_channel::ipc::TryRecvError;
use client::ipc_channel::ipc::IpcSender as Sender;

use message;
use message::MessageType;
use message::ProtocolMessage;

// Client state and primitives for communicating with the coordinator
#[derive(Debug)]
pub struct Client {
    pub id_str: String,
    pub running: Arc<AtomicBool>,
    pub num_requests: u32,
    pub to_coord : Sender<ProtocolMessage>,
    pub from_coord : Receiver<ProtocolMessage>,
    pub pending_req : bool,
    pub commit_ops : u64,
    pub abort_ops : u64,
    pub unknown_ops : u64,
}

///
/// Client Implementation
/// Required:
/// 1. new -- constructor
/// 2. pub fn report_status -- Reports number of committed/aborted/unknown
/// 3. pub fn protocol(&mut self, n_requests: i32) -- Implements client side protocol
///
impl Client {

    ///
    /// new()
    ///
    /// Constructs and returns a new client, ready to run the 2PC protocol
    /// with the coordinator.
    ///
    /// HINT: You may want to pass some channels or other communication
    ///       objects that enable coordinator->client and client->coordinator
    ///       messaging to this constructor.
    /// HINT: You may want to pass some global flags that indicate whether
    ///       the protocol is still running to this constructor
    ///
    pub fn new(id_str: String,
               s : Sender<ProtocolMessage>,
               r : Receiver<ProtocolMessage>,
               running: Arc<AtomicBool>) -> Client {
        Client {
            id_str: id_str,
            running: running,
            num_requests: 0,
            to_coord : s,
            from_coord : r,
            pending_req : false,
            commit_ops : 0,
            abort_ops : 0,
            unknown_ops : 0,
        }
    }

    ///
    /// wait_for_exit_signal(&mut self)
    /// Wait until the running flag is set by the CTRL-C handler
    ///
    pub fn wait_for_exit_signal(&mut self) -> bool {
        trace!("Client-{}::Waiting for exit signal", self.id_str.clone());

		// Wait until the flag is set.
		if self.running.load(Ordering::SeqCst) == true {
            return false;
        }

        trace!("Client-{}::Exiting", self.id_str.clone());

        return true;
    }

    ///
    /// send_next_operation(&mut self)
    /// Send the next operation to the coordinator
    ///
    pub fn send_next_operation(&mut self, mtype : MessageType) {

        // Create a new request with a unique TXID.
        self.num_requests = self.num_requests + 1;
        let txid = format!("{}_op_{}", self.id_str.clone(), self.num_requests);
        let pm = message::ProtocolMessage::generate(mtype,
                                                    txid.clone(),
                                                    self.id_str.clone(),
                                                    self.num_requests);
        info!("{}::Sending operation #{}", self.id_str.clone(), self.num_requests);

        if self.wait_for_exit_signal() != true {
            self.to_coord.send(pm).unwrap();
            self.pending_req = true;
        }

        trace!("{}::Sent operation #{}", self.id_str.clone(), self.num_requests);
    }

    ///
    /// recv_result()
    /// Wait for the coordinator to respond with the result for the
    /// last issued request. Note that we assume the coordinator does
    /// not fail in this simulation
    ///
    pub fn recv_result(&mut self) {

        info!("{}::Receiving Coordinator Result", self.id_str.clone());

        if self.running.load(Ordering::SeqCst) != true {
            return;
        }
        match self.from_coord.try_recv() {
            Ok(msg) => {
                self.pending_req = false;
                if msg.mtype == MessageType::ClientResultCommit {
                    self.commit_ops += 1;
                } else if msg.mtype == MessageType::ClientResultAbort {
                    self.abort_ops += 1;
                } else {
                    self.unknown_ops += 1;
                }
            },
            Err(e) => {
                match e {
                TryRecvError::Empty => {
                    // there are no more messages ready immediately,
                },
                _ => ()
                }
            }
        }
    }

    ///
    /// report_status()
    /// Report the abort/commit/unknown status (aggregate) of all transaction
    /// requests made by this client before exiting.
    ///
    pub fn report_status(&mut self) {
        // TODO: Collect actual stats
        let successful_ops: u64 = self.commit_ops;
        let failed_ops: u64 = self.abort_ops;
        let unknown_ops: u64 = self.unknown_ops;

        println!("client_{}       :\tCommitted: {:6}\tAborted: {:6}\tUnknown: {:6}", self.id_str.clone(), successful_ops, failed_ops, unknown_ops);
    }

    ///
    /// protocol()
    /// Implements the client side of the 2PC protocol
    /// HINT: if the simulation ends early, don't keep issuing requests!
    /// HINT: if you've issued all your requests, wait for some kind of
    ///       exit signal before returning from the protocol method!
    ///
    pub fn protocol(&mut self, n_requests: u32) {
        loop {
            let exit = false;
            if self.num_requests != n_requests {
                self.send_next_operation(MessageType::ClientRequest);
            }
            while self.pending_req {
                thread::sleep(Duration::from_millis(100));
                self.recv_result();
                if self.wait_for_exit_signal() == true {
                    break;
                }
            }
            if exit == true {
                break;
            }
            if self.num_requests == n_requests {
                self.send_next_operation(MessageType::CoordinatorExit);
                thread::sleep(Duration::from_millis(100));
                break;
            }
        }
        self.report_status();
    }
}
