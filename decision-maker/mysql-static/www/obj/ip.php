<?php
class OBJ_IP extends DB {
	public $_ip;
	public $_mac;
	public $_id;
	public $_group_id;
	public $table;
	function __construct($id = false) {
		$this->table = "IP"; /* XXX */
		parent::__construct();
		if ($id) {
			$this->get($id);
		}
	}
	function getgid($gid) {
		$gid = intval($gid); /* validate? */
		$query = "SELECT * FROM `{$this->table}` WHERE `group_id` = $gid";
		$ret = array();
		foreach ($this->query($query) as $row) {
			$i = new OBJ_IP();
			$i->fill($row);
			array_push($ret,$i);
		}
		return $ret;
	}
	function move_to_gid($gid) {
		if (!$this->valid_self()) {
			return;
		}
		$gid = intval($gid);
		$g = new OBJ_GROUP($gid);
		if ($g->valid_self()) {
			$query = "UPDATE `{$this->table}` SET group = $gid WHERE id= {$this->_id}";
			if (!$this->query($query)) {
				$this->warn('update failed');
			}
		}
	}
	function valid_group($id) {
		$g = new OBJ_GROUP($id);
		if (!$g->valid_self()) {
			$this->warn("invalid group $id");
			return false;
		}
		return true;
	}
	function valid_self($validate_id = true) {
		if ($validate_id) {
			if (!$this->valid_id($this->_id)) {
				return false;
			}
		}
		if ($this->valid_ip($this->_ip) &&
			$this->valid_group($this->_group_id) &&
			$this->valid_mac($this->_mac)) {
				return true;
		}
		return false;
	}
}
?>