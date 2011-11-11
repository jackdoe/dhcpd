<?php
class unknown extends PRIMITIVE {
	function __construct($facade) {
		parent::__construct($facade);
		$this->template = 'output.tpl';
	}
	function unknown() {
		
	}
	
	function __destruct() {
		parent::__destruct();
	}
}
?>
