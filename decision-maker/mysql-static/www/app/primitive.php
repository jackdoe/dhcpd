<?php 
class PRIMITIVE extends ERR {
	public $template;
	public $facade;
	public $message;
	public $rc;
	function __construct($facade) {
		$this->facade = $facade;
		$this->message = "";
		$this->rc = UNDEFINED; /* success */
	}
	function __call($method, $params) {
		/* undefined method call, 302? */
		$this->warn("dont know how to do: $method");
	}
	function __destruct() {
	
	}
	function submenu($factory_name) {
		
	}
	function say($msg) {
		$this->message .= $msg;
	}
	function success($msg) {
		$this->say($msg);
		$this->rc = SUCCESS;
	}
	function fail($msg) {
		$this->say($msg);
		$this->rc = FAIL;
	}
	function pcount() {
		return count($this->facade->params);
	}
}
?>