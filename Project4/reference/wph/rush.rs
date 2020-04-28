
extern crate libc;
use std::ffi::{CString, CStr};
use std::os::raw::c_char;

use std::io;
use std::str::FromStr;
use std::io::Write;

use std::path::{Path, PathBuf};

macro_rules! _cstr{
	($s:expr) => (CString::new($s).unwrap());
}

macro_rules! _cstrp{
	($s:expr) => (_cstr!($s).as_ptr());
}

macro_rules! _rstr{
 	($s:expr) => (CStr::from_ptr($s).to_string_lossy().into_owned());
}

struct PathUtils;

impl PathUtils{

	fn concat(path0: &str, path1: &str) -> String{
		let mut path_buf = PathBuf::from(path0);
		path_buf.push(path1);
		return String::from(path_buf.to_str().unwrap());
	}

}

struct Command{
	m_args: Vec<String>
	, m_output: String
	, m_input: String
}

impl Command{

	fn new_empty() -> Command{
		return Command{m_args: Vec::new(), m_output: String::new(), m_input: String::new()};
	}

	fn is_empty(&self) -> bool{
		return self.m_args.is_empty();
	}

	fn command_name(&self) -> &str{
		return self.m_args[0].as_str();
	}

	fn input_file(&self) -> &str{
		return self.m_input.as_str();
	}

	fn output_file(&self) -> &str{
		return self.m_output.as_str();
	}

}

struct CommandLine{
	m_commands: Vec<Command>
	, m_background: bool
	, m_text: String
}

impl CommandLine{

	fn new(line: &str) -> CommandLine{
		let mut new = CommandLine{m_commands: Vec::new(), m_background: false, m_text: String::new()};

		let mut tokens:Vec<&str> = line.split_whitespace().collect();
		let mut command = Command::new_empty();

		new.m_background = !tokens.is_empty() && tokens[tokens.len() - 1] == "&";
		if new.m_background{
			tokens.pop();
		}

		new.m_text = tokens.join(" ");

		let mut wait_for_input = false;
		let mut wait_for_output = false;
		for token in &tokens{
			if *token == "|"{
				if !command.is_empty(){
					new.m_commands.push(command);
				}
				command = Command::new_empty();
			}else if *token == "<"{
				wait_for_input = true;
			}else if *token == ">"{
				wait_for_output = true;
			}else{
				if wait_for_input{
					command.m_input = String::from(*token);
					wait_for_input = false;
				}else if wait_for_output{
					command.m_output = String::from(*token);
					wait_for_output = false
				}else{
					command.m_args.push(String::from(*token));
				}
			}
		}
		if !command.is_empty(){
			new.m_commands.push(command);
		}
		return new;
	}

	fn single_command(&self) -> bool{
		return self.m_commands.len() == 1;
	}

	fn is_empty(&self) -> bool{
		return self.m_commands.is_empty();
	}

	fn serialize(&self) -> &str{
		return self.m_text.as_str();
	}

}

struct Job{
	m_pid: i32
	, m_command_line: String
}

impl Job{

	fn new(pid:i32, command_line:&str) -> Job{
		let v:Vec<&str> = command_line.split_whitespace().collect();
		return Job{m_pid: pid, m_command_line: String::from(v.join(" "))};
	}

}

struct Dash{
	m_jobs: Vec<Job>
	, m_history: Vec<String>
}

impl Dash{

	fn new() -> Dash{
		return Dash{m_jobs: Vec::new(), m_history: Vec::new()};
	}

	fn deal(&mut self, input: &str){
		// let mut input = String::new();
		// io::stdin().read_to_string(&mut input).expect("Failed to read string");
		// let lines:Vec<&str> = input.lines().collect();
		// for line in lines{
		let lines:Vec<&str> = input.lines().collect();
		for line in lines{
			//let mut input = String::new();
			//io::stdin().read_to_string(&mut input).expect("Failed to read string");
			//println!("{}", input);

			let mut child_process = false;
			unsafe{
				let mut command_line = CommandLine::new(line);
				if !command_line.is_empty(){
					let single_buildin = command_line.single_command() && self.exec_buildin(&command_line.m_commands[0]);
					if !single_buildin{
						let pid = libc::fork();
						child_process = pid == 0;
						if child_process{
							self.do_child_process(&mut command_line, -1, -1);
						}else{
							self.m_jobs.push(Job::new(pid, command_line.serialize()));
							if !command_line.m_background{
								let mut status:i32 = 0;
								let option = 0;
								while libc::waitpid(pid, &mut status, option) < 0{
									//Waiting
								}
							}
						}
					}
				}
			}
			if !child_process{
				self.m_history.push(String::from(line));
			}

		}
	}

	unsafe fn cd(&mut self, dir: &str){
		let path_buf = std::env::current_dir().unwrap();
		let current_dir = path_buf.to_str().unwrap();
		libc::chdir(_cstrp!(PathUtils::concat(current_dir, dir).as_str()));
	}

	unsafe fn pwd(&self){
		println!("{}", std::env::current_dir().unwrap().to_str().unwrap());
	}

	unsafe fn history(&self){
		let mut i:i32 = 1;
		for s in &self.m_history{
			let mut line = String::new();
			let num = i.to_string();
			line.push_str(vec![" ";5 - num.len()].concat().as_str());
			line.push_str(num.as_str());
			line.push_str("  ");
			line.push_str(s.as_str());
			println!("{}", line);
			i += 1;
		}
	}

	unsafe fn jobs(&mut self){
		let mut other:Vec<Job> = Vec::new();
		for job in &self.m_jobs{
			let pid = job.m_pid;
			let mut status:i32 = 0;
			let option = libc::WNOHANG;
			let res = libc::waitpid(pid, &mut status, option);
			if res == 0{
				other.push(Job::new(pid, job.m_command_line.as_str()));
				println!("{}", job.m_command_line);
			}
		}
		self.m_jobs = other;
	}

	unsafe fn kill(&self, pid: i32){
		libc::kill(pid, libc::SIGTERM);
	}

	unsafe fn exit(&self){
		libc::exit(0);
	}

	unsafe fn exec_buildin(&mut self, command: &Command) -> bool{
		let mut buildin = true;
		let command_name = command.command_name();
		if command_name == "cd"{
			self.cd(command.m_args[1].as_str());
		}else if command_name == "history"{
			self.history();
		}else if command_name == "jobs"{
			self.jobs();
		}else if command_name == "kill"{
			self.kill(i32::from_str(command.m_args[1].as_str()).unwrap());
		}else if command_name == "pwd"{
			self.pwd();
		}else if command_name == "exit"{
			self.exit();
		}else{
			buildin = false;
		}
		return buildin;
	}

	unsafe fn exec(&mut self, command: &Command){
		if self.exec_buildin(command){
			//2017.10.16 Exit child process
			libc::exit(0);
		}else{
			let mut command_name = String::from(command.command_name());
			if command_name.as_str().find("/").is_none(){
				let env_str = _rstr!(libc::getenv(_cstrp!("PATH")));
				let envs:Vec<&str> = env_str.split(":").collect();
				for env_path in &envs{
					let abs_path = PathUtils::concat(env_path, &command_name);
					if Path::new(&abs_path).exists(){
						command_name = abs_path;
						break;
					}
				}
			}

			let mut v1:Vec<CString> = Vec::new();
			for token in &command.m_args{
				v1.push(_cstr!(token.as_str()));
			}
			let mut v2:Vec<*const c_char> = Vec::new();
			for cstr in &v1{
				v2.push(cstr.as_ptr());
			}
			v2.push(std::ptr::null());
			libc::execv(_cstrp!(command_name.as_str()), v2.as_ptr());
		}
	}

	unsafe fn do_child_process(&mut self, commands: &mut CommandLine, pfd_input: i32, pfd_output: i32){
		if let Some(current_command) = commands.m_commands.pop(){
			let mut pfds:Vec<i32> = vec![-1;2];
			let mut child_process = false;
			if !commands.is_empty(){
				if libc::pipe((&mut pfds).as_mut_ptr()) == 0{
					child_process = libc::fork() == 0;
					if child_process{
						//2017.10.15 Child process
						self.do_child_process(commands, pfds[0], pfds[1]);
					}
				}
			}

			if !child_process{
				//2017.10.15 Parent process

				let input_file = current_command.input_file();
				if !input_file.is_empty(){
					//2017.10.15 Input redirection
					let input_fd = libc::open(_cstrp!(input_file), libc::O_RDONLY);
					libc::dup2(input_fd, libc::STDIN_FILENO);
					libc::close(input_fd);
				}else if pfds[0] >= 0 && pfds[1] >= 0{
					//2017.10.15 Pipe stdin from child process
					libc::close(libc::STDIN_FILENO);
					libc::dup2(pfds[0], libc::STDIN_FILENO);
					libc::close(pfds[1]);
				}

				let output_file = current_command.output_file();
				if !output_file.is_empty(){
					//2017.10.15 Ouput redirection
					let open_flags = libc::O_RDWR | libc::O_TRUNC | libc::O_CREAT;
					let create_mode = libc::S_IRUSR | libc::S_IWUSR | libc::S_IRGRP | libc::S_IWGRP | libc::S_IROTH;
					let output_fd = libc::open(_cstrp!(output_file), open_flags, create_mode);
					libc::dup2(output_fd, libc::STDOUT_FILENO);
					libc::close(output_fd);
				}else if pfd_output >= 0 && pfd_input >= 0{
					//2017.10.15 Pipe stdout to parent process
					libc::close(libc::STDOUT_FILENO);
					libc::dup2(pfd_output, libc::STDOUT_FILENO);
					libc::close(pfd_input);
				}
				self.exec(&current_command);
			}
		}
	}

}

fn main()
{
	let mut input = String::new();
	let mut dash = Dash::new();
	loop{
		print!("$ ");
		io::stdout().flush().expect("Flush output error");
		io::stdin().read_line(&mut input).expect("Read line error");

		if input.is_empty(){
			break;
		}
		dash.deal(input.as_str());
		input.clear();
	}
}
