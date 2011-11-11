<?php
class auth extends PRIMITIVE {
	public $u;
	function __construct($facade) {
		parent::__construct($facade);
		$this->u = new OBJ_USER();
		$this->template = 'login.tpl';
	}
	function login() {
		if (array_key_exists('user',$_POST) && array_key_exists('pass',$_POST)) {
			$user = $_POST{'user'};
			$pass = $_POST{'pass'};
			$this->u->auth($user,$pass);
			home();
		}
	}
	function unknown($params) {
		return $this->login();
	}
	function logout() {
		$this->u->logout();
		home();
	}
	
	function __destruct() {
		parent::__destruct();
	}
}
?>
