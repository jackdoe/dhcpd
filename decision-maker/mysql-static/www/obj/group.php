<?php
class OBJ_GROUP extends DB {
	public $_id;
	public $_descr;
	public $table;
	function __construct($id = -1) {
		parent::__construct();
		$this->table = 'GROUP';
		if ($id > 0) {
			$this->get($id);
//		} else {
//			$this->_id = 0;
//			$this->_descr = "";
		}
	}
	function valid_self($validate_id = true) {
		if ($validate_id && !$this->valid_id($this->_id)) 
			return false;
		if ($this->valid_string($this->_descr,MIN_GROUP_DESCRIPTION)) 
			return true;
		return false;
	}
	function members() {
		if (!$this->valid_self())
			return array();
		$i = new OBJ_IP();
		return $i->getgid($this->_id);
	}
	
}
?>