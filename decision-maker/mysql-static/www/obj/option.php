<?php 
class OBJ_OPTION extends DB {
	public $_id;
	public $_owner;
	public $_type;
	public $_len;
	public $_value;
	public $_obj_parser;
	public $_obj_count;
	public $_descr;
	public $table;
	function __construct($id = false) {
		parent::__construct();
		$this->table = 'OPTION';
		if ($id) {
			$this->get($id);
		}	
	}
	function valid_id($id) {
		if (is_int($id) && intval($id) > 0) {
			return true;
		}
		return false;
	}
	function valid_self($validate_id = true) {
		if ($validate_id) {
			if (!$this->valid_id($this->_id)) {
				return false;
			}
		}
		if ($this->valid_nonzero($this->_id) &&
			$this->valid_nonzero($this->_owner) &&
			$this->valid_nonzero($this->_type) &&
			$this->valid_nonzero($this->_len) &&
			$this->valid_nonzero($this->_obj_count) &&
			$this->valid_string($this->_obj_parser) &&
			$this->valid_string($this->_value) &&
			$this->valid_string($this->_descr)) {
				return true;
		}
		return false;
	}
}
?>