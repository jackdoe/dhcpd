<?php
$link = NULL;
class DB extends ERR {
	public $pdo;
	function __construct() {
		global $link;
		if (!$link) {
			$link = new PDO('mysql:unix_socket=/tmp/mysql.sock;dbname=dhcp','dhcp','*SA&DCXHZ^@SAJDJ');
			$link->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			
		}
		if ($link) {
			$this->pdo = $link;
		} else {
			die('db fail');
		}
	}
	function last_id($name = 'id') {
		return $this->pdo->lastInsertId($name);
	}
	function quote($q) {
		return $this->pdo->quote($q);
	}
	function query_and_fill($q) {
		$obj = $this->query($q);
//		var_dump($obj);
		if ($obj) {
			foreach ($obj as $row) {
				$this->fill($row);
			}
		}
	}
	function fill($row) {
		$vars = get_object_vars($this);
		foreach ($row as $k=>$v) {
			$k = "_$k";
			if (array_key_exists($k,$vars)) {
				if (is_numeric($v))
					$this->$k = intval($v);
				else
					$this->$k = $v;
			} else {
				$this->warn("$k not found in get_object_vars for class:" . get_class($this));
			}
		}
	}
	function query($q) {
		//var_dump($q);
		$r = NULL;
		try {
			$r = $this->pdo->query($q,PDO::FETCH_ASSOC);
		} catch(Exception $e) {
			$this->warn("executing $q Exception:");
			$this->warn($e->getMessage());
			$this->warn($this->trace());
		};
		return $r;
	}

	function delete() {
		if (!$this->valid_self()) 
			return false;
		
		$id = intval($this->_id);
		$query = "DELETE FROM `{$this->table}` WHERE id=$id";
		if (!$this->query($query)) 
			$this->warn('delete failed');
		return true;
		
	}
	function update() {
		if (!$this->valid_self()) 
			return false;
		
		$vars = get_object_vars($this);
		$set = array();
		foreach($vars as $v=>$val) {
			if (preg_match('/^_/',$v)) {
				if (is_int($this->$v)) {
 					$value = intval($this->$v);
 				} else {
 					$value = $this->quote($this->$v);
 				}
 				$v = preg_replace('/^_/',"",$v);
 				array_push($set,"`$v` = $value");
			}
		}
		if (count($set) > 0) {
			$id = intval($this->_id);
			$query = "UPDATE `{$this->table}` SET ". implode(",",$set) ." WHERE id=$id";
			if (!$this->query($query)) {
				$this->warn('update failed');
			} else {
				return true;
			}
		}
	}
	function insert() {
		if (!$this->valid_self(false)) {
			return false;
		}
		$vars = get_object_vars($this);
		$objects = array();
		$values = array();
		$i=0;
		foreach ($vars as $v=>$val) {
		
			if (preg_match('/^_/',$v) && $v != '_id') {
				if (is_int($this->$v)) {
 					$value = intval($this->$v);
 				} else {
 					$value = $this->quote($this->$v);
 				}
				$v = preg_replace('/^_/',"",$v);
				$objects[$i] = "`$v`";
				$values[$i] = $value;
				$i++;
			}
		}
		if ($i > 0) {
			$query = "INSERT INTO `{$this->table}` (".implode(",",$objects).") VALUES(".implode(",",$values).")";
			if ($this->query($query)) {
				$this->get($this->last_id());
				return true;
			} else {
				$this->warn('insert failed');
			}
		}
	}
	
	function get($id) {
		$id = intval($id);
		if ($this->valid_id($id)) { 
			$this->query_and_fill("SELECT * FROM `{$this->table}` WHERE id=$id");
//		} else {
//			$this->warn("bad id $id");
		}
	}
	function getAll() {
		$query = "SELECT * FROM `{$this->table}`";
		$ret = array();
		$c = get_class($this);
		foreach ($this->query($query) as $row) {
			$i = new $c();
			$i->fill($row);
			array_push($ret,$i);
		}
		return $ret;
	}
	function assoc() {
		$assoc = array();
		$vars = get_object_vars($this);
		foreach ($vars as $v=>$val) {
			if (preg_match('/^_/',$v)) {
				$assoc{$v} = $this->$v;
			}
		}
		return $assoc;
	}
}
?>