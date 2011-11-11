<?php 
class PRIMITIVE extends ERR {
	public $template;
	public $facade;
	public $message;
	function __construct($facade) {
		$this->facade = $facade;
		$this->message = "";
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
}
?>