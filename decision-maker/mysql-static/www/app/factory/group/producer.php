<?php
class group extends PRIMITIVE {
	function __construct($facade) {
		parent::__construct($facade);
		$this->template = 'output.tpl';
	}
	function unknown() {
		$this->dump();		
	}
	function edit() {
		$this->template = 'edit.tpl';
		$g = new OBJ_GROUP();
		$out = $g->assoc();
		if (count($this->facade->params) == 2) {
			$id = intval(array_shift($this->facade->params));
			$action = array_shift($this->facade->params);
			if ($id == 0) {
				$method = "insert";
				$validate_id = false;
			} else {
				$method = "update";
				$validate_id = true;
			}
			$g->get($id);
			switch($action) {
			case "try":
			break;
			case "commit":
				if (isset($_POST{'descr'})) {
					$g->_descr = $_POST{'descr'};
					if ($g->$method()) 
						$this->say("$method: success");
					else
						$this->say("$method: failed");
				}
			break;
			}
			$out = $g->assoc();
		}
		$this->facade->smarty->assign('group',$out);
	}
	function delete() {
		$out = array();
		$this->template = 'delete.tpl';
		if (count ($this->facade->params) == 2) {
			$id = intval(array_shift($this->facade->params));
			$action = array_shift($this->facade->params);
			$g = new OBJ_GROUP($id);
			switch ($action) {
			case 'try':
				$out = $g->assoc();
				break;
			case 'commit':
				$c = 0;
				foreach ($g->members() as $ip) {
					$ip->move_to_gid(ANONYMOUS_GROUP);
					$c++;
				}
				if ($g->delete()) 
					$this->say("delete: success");
				else
					$this->say("delete: failed");
				if ($c > 0) {
					$this->say(", $c members moved from gid $id to " . ANONYMOUS_GROUP);
				}
				break;
			}
		}	
		$this->facade->smarty->assign('group',$out);
	}
	function members() {
		$this->template = 'members.tpl';
		$out = array();
		if (count($this->facade->params >= 2)) {
			$id = intval(array_shift($this->facade->params));
			$action = array_shift($this->facade->params);
			switch ($action) {
			case 'view':
				$g = new OBJ_GROUP($id);
				foreach ($g->members() as $ip) {
					array_push($out,$ip->assoc());
				}
			break;
			case 'remove':
			
			break;
			case 'add':
			break;
			}
		}
		$this->facade->smarty->assign('ips',$out);
	}
	function dump() {
		$this->template = 'dump.tpl';
		$g = new OBJ_GROUP();
		$output = array();
		foreach ($g->getAll() as $group) {
			array_push($output,$group->assoc());
		}
		$this->facade->smarty->assign('groups',$output);
	}
	function __destruct() {
		parent::__destruct();
	}
	function submenu($factory_name) {
		$ret = array();
		$site = SITE . "/{$this->facade->factory_name}";
		array_push($ret,array('href' => "$site/dump/",'text' => '/dump'));
		array_push($ret,array('href' => "$site/edit/0/try",'text' => '/add'));

		return $ret;
	}
}
?>
