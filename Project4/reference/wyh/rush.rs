///////////////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************************

- Author: Yuehao Wang
- Latest update: 11/01/2017
- Child process structure:
    
            main process
                |
    -------------------...--------
   |            |                |     
  child 1    child 2  ...    child n   


******************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////


extern crate libc;

use libc::*;
use std::ffi::{CString};
use std::io;
use std::io::{Write};


const CRASH_MSG: &str = "Aw, Rush crashed...";
const BUILTIN_CMDS: [&str; 6] = ["cd", "pwd", "history", "jobs", "kill", "exit"];


#[derive(Debug)]
struct Command {
    cmd_str: String,
    is_backend: bool,
    is_only_command: bool
}

#[derive(Debug)]
struct Pid {
    cmd_ln_str: String,
    pids: Vec<i32>
}
    

#[derive(Debug)]
struct Rush {
    history: Vec<String>,
    pid_ls: Vec<Pid>
}

impl Rush {
    fn new() -> Rush {
        Rush {
            history: vec![],
            pid_ls: vec![]
        }
    }

    unsafe fn parse_cmd_ln(&mut self, cmd_ln_str: &String) {
        let mut is_backend: bool = false;
        let mut cmd_ln_str_trimmed: String = String::from(cmd_ln_str.trim());

        if cmd_ln_str_trimmed.len() > 0 {
            if cmd_ln_str_trimmed.ends_with("&") {
                is_backend = true;

                cmd_ln_str_trimmed = String::from(cmd_ln_str_trimmed[0..(cmd_ln_str_trimmed.len() - 1)].trim());
            }

            let cmd_str_ls: Vec<&str> = cmd_ln_str_trimmed.split("|").collect();

            let mut cmd_ls: Vec<Command> = vec![];

            let mut i: usize = 0;

            while i < cmd_str_ls.len() {
                cmd_ls.push(Command {
                    cmd_str: String::from(cmd_str_ls[i]),
                    is_backend: is_backend,
                    is_only_command: cmd_str_ls.len() == 1
                });

                i += 1;
            }

            let mut pid_obj = Pid {
                cmd_ln_str: cmd_ln_str_trimmed.clone(),
                pids: vec![]
            };

            self.run(&mut cmd_ls, None, &mut pid_obj);

            self.pid_ls.push(pid_obj);
        }
        

        self.history.push(cmd_ln_str.clone());
    }

    unsafe fn run(&mut self, cmd_ls: &mut Vec<Command>, pre_pipe: Option<[i32; 2]>, pid_obj: &mut Pid) {
        let cmd_str: String = (&cmd_ls[0].cmd_str).clone();
        let is_backend: bool = cmd_ls[0].is_backend;

        let mut str_ls: Vec<&str> = cmd_str.split_whitespace().collect();

        if str_ls.len() <= 0 {
            return;
        }


        let mut output_file: &str = "";
        let mut input_file: &str = "";

        let mut str_ls_i: usize = 0;
        while str_ls_i < str_ls.len() {
            let s: &str = str_ls[str_ls_i];

            if s == "<" {
                str_ls.remove(str_ls_i);

                input_file = str_ls.remove(str_ls_i);

                str_ls_i -= 1;
            } else if s == ">" {
                str_ls.remove(str_ls_i);

                output_file = str_ls.remove(str_ls_i);

                str_ls_i -= 1;
            }

            str_ls_i += 1;
        }

        if cmd_ls.len() > 1 || !BUILTIN_CMDS.contains(&str_ls[0]) || !cmd_ls[0].is_only_command {
            let mut pipe_hd: [i32; 2] = [0, 0];
            pipe(pipe_hd.as_mut_ptr());

            let child: i32 = fork();

            if child != 0 {
                if !pre_pipe.is_none() {
                    let pre_pipe_som: [i32; 2] = pre_pipe.unwrap();
                    close(pre_pipe_som[0]);
                    close(pre_pipe_som[1]);
                }

                if !is_backend {
                    let mut status: i32 = 1;
                    waitpid(child, &mut status, 0);
                } else {
                    pid_obj.pids.push(child);
                }

                cmd_ls.remove(0);
                if cmd_ls.len() > 0 {
                    self.run(cmd_ls, Some(pipe_hd), pid_obj);
                }
            } else {
                let i_file_handle: i32 = open(CString::new(input_file).unwrap().as_ptr(), O_RDONLY);
                let o_file_handle: i32 = open(CString::new(output_file).unwrap().as_ptr(), O_CREAT | O_RDWR | O_TRUNC, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) as isize);
                dup2(i_file_handle, STDIN_FILENO);
                dup2(o_file_handle, STDOUT_FILENO);

                if cmd_ls.len() > 1 {
                    close(pipe_hd[0]);
                    close(STDOUT_FILENO);
                    dup2(pipe_hd[1] , STDOUT_FILENO);
                    close(pipe_hd[1]);
                }

                if !pre_pipe.is_none() {
                    let pre_pipe_som: [i32; 2] = pre_pipe.unwrap();
                    close(pre_pipe_som[1]);
                    close(STDIN_FILENO);
                    dup2(pre_pipe_som[0] , STDIN_FILENO);
                    close(pre_pipe_som[0]);
                }

                self.execute_cmd(str_ls);

                close(i_file_handle);
                close(o_file_handle);

                exit(0);
            }
        } else {
            self.execute_cmd(str_ls);
        } 
    }

    unsafe fn execute_cmd(&self, str_ls: Vec<&str>) {
        match str_ls[0] {
            "cd" => {
                if str_ls.len() <= 1 {
                    return;
                }

                self.exec_cmd_cd(str_ls[1]);
            },
            "pwd" => {
                self.exec_cmd_pwd();
            },
            "exit" => {
                self.exec_cmd_exit();
            },
            "history" => {
                self.exec_cmd_history();
            },
            "jobs" => {
                self.exec_cmd_jobs();
            },
            "kill" => {
                if str_ls.len() <= 1 {
                    return;
                }

                self.exec_cmd_kill(str_ls[1]);
            },
            _ => {
                self.exec_external_cmd(str_ls);
            }
        }
    }

    unsafe fn exec_cmd_cd(&self, path_str: &str) {
        chdir(CString::new(path_str).unwrap().as_ptr());
    }

    unsafe fn exec_cmd_pwd(&self) {
        println!("{}", std::env::current_dir().unwrap().to_str().unwrap());
    }

    unsafe fn exec_cmd_exit(&self) {
        exit(0);
    }

    unsafe fn exec_cmd_history(&self) {
        for (i, item) in self.history.iter().enumerate() {
            print!("{index:>width$}", index = i + 1, width = 5);
            print!("  {}", item);
        }
    }

    unsafe fn exec_cmd_jobs(&self) {
        let mut i = 0;

        while i < self.pid_ls.len() {
            let pid_obj: &Pid = &self.pid_ls[i];

            for pid in &pid_obj.pids {
                let mut status: i32 = 1;

                if waitpid(*pid, &mut status, 1) == 0 {
                    println!("{}", String::from(pid_obj.cmd_ln_str.split_whitespace().collect::<Vec<&str>>().join(" ")));

                    break;
                }
            }
            
            i += 1;
        }
    }

    unsafe fn exec_cmd_kill(&self, pid_str: &str) {
        match pid_str.parse::<i32>() {
            Ok(pid) => {
                kill(pid, SIGTERM);
            },

            Err(_) => ()
        }
    }

    unsafe fn exec_external_cmd(&self, str_ls: Vec<&str>) {
        let mut arg_vec: Vec<CString> = Vec::new();
        arg_vec.push(CString::new(str_ls[0]).unwrap());

        for arg in &str_ls[1..] {
            arg_vec.push(CString::new(*arg).unwrap());
        }

        let mut cvec: Vec<*const c_char> = Vec::new();
        for cstr in &arg_vec{
            cvec.push(cstr.as_ptr());
        }
        cvec.push(std::ptr::null());

        execvp(CString::new(str_ls[0]).unwrap().as_ptr(), cvec.as_ptr());
    }
}

fn main() {
    let mut bash: Rush = Rush::new();

    loop {
        print!("$ ");
        io::stdout().flush().unwrap();

        let mut input: String = String::new();
        io::stdin().read_line(&mut input).expect(CRASH_MSG);

        if input.len() <= 0 {
            break;
        }

        unsafe { bash.parse_cmd_ln(&input); }
    }
}
