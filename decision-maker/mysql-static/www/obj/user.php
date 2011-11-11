<?php 
class OBJ_USER extends DB {
	public $_id;
	public $_username;
	public $_password;
	public $_priv_level;
	function __construct($id = false) {
		parent::__construct();
		$this->table = 'USER';
		if ($id) {
			$this->get($id);
		}	
	}
	function valid_self($validate_id = false) {
		if ($validate_id) {
			if (!$this->valid_id($this->_id)) {
				return false;
			}
		}
		if ($this->valid_string($this->_username) &&
			$this->valid_string($this->_password)) {
				return true;
		}
		return false;	
	}	
	function get_user($user) {
		$quoted_username = $this->quote($user);
		$query = "SELECT * FROM `{$this->table}` WHERE username = $quoted_username";
		$this->query_and_fill($query);
	}
	
	function auth($user,$pass) {
		$this->get_user($user);
		if (ENCRYPTED_PASSWORDS) {
			$pass = sha1($pass);
		}
		if ($this->valid_self() &&
			$this->_username == $user && 
			$this->_password == $pass) { 
			$this->login();
			return true;
		} else {
			$this->logout();
			return false;
		}
	}
	function need_auth($factory_name) {
		if (isset($_SESSION{'last_access'}) && 
			(time() - intval($_SESSION{'last_access'})) < SESSION_EXPIRE) {
				$_SESSION{'last_access'} = time();
				return false;
		}
		return true;
	}
	
	function logout() {
		$_SESSION{'last_access'} = 0;
	}
	function login() {
		$_SESSION{'last_access'} = time();
		$_SESSION{'logged_user'} = $this->_username;
	}
}
?>