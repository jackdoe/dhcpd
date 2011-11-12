<?php
class group extends PRIMITIVE {
	function __construct($facade) {
		parent::__construct($facade);
		$this->template = 'output.tpl';
	}
	function unknown() {
		$this->dump();		
	}
	function edit($id = 0 ,$action = 'unknown', $subid = 0, $subaction = 'unknown') {
		$this->template = 'edit.tpl';
		$g = new OBJ_GROUP($id);
		$anon = new OBJ_GROUP();
		$anon->get_anon();
		if ($g->valid_id($g->_id) && $g->_id == $anon->_id) {
			$this->fail("cant edit " . ANONYMOUS_GROUP);
			return;
		}

		$out = array();
		$out = $g->assoc();
		if ($id == 0) {
			$method = "insert";
			$validate_id = false;
		} else {
			$method = "update";
			$validate_id = true;
		}
		switch($action) {
		case 'try':
			$out = $g->assoc();
		break;
		case 'commit':
			$descr = (isset($_POST{'descr'})) ? $_POST{'descr'} : '';
			$g->_descr = $descr;
			if ($g->$method())
				$this->success($method);
			else
				$this->fail($method);
		break;
		}
		$this->facade->smarty->assign('group',$out);
	}
	function delete($id = 0 ,$action = 'unknown', $subid = 0, $subaction = 'unknown') {
		$this->template = 'delete.tpl';
		$g = new OBJ_GROUP($id);
		$anon = new OBJ_GROUP();
		$anon->get_anon();
		if ($g->valid_id($g->_id) && $g->_id == $anon->_id) {
			$this->fail("cant remove " . ANONYMOUS_GROUP);
			return;
		}
		$out = array();
		switch ($action) {
		case 'try':
			$out = $g->assoc();
			break;
		case 'commit':
			$c = 0;
			foreach ($g->members() as $ip) {
				$ip->move_to_gid($anon->_id);
				$c++;
			}
			if ($g->delete()) {
				$this->success("deleted group $id with " . (($c > 0) ? (($c > 1) ? "$c members" : "1 member") . "(moved to ".ANONYMOUS_GROUP.")" : "zero members"));
			} else {
				$this->fail("delete failed");
			}
			break;
		}
		$this->facade->smarty->assign('group',$out);
	}
	function members($id = 0 ,$action = 'view', $subid = 0, $subaction = 'unknown') {
		$this->template = 'members.tpl';
		$anon = new OBJ_GROUP();
		$anon->get_anon();
		switch ($action) {
		case 'view':
			$g = new OBJ_GROUP($id);
			$out = array();
			$anon_memb = array();
			foreach ($g->members() as $ip) {
				array_push($out,$ip->assoc());
			}
			if ($id != $anon->_id) {
				foreach ($anon->members() as $ip) {
					array_push($anon_memb,$ip->assoc());
				}
			}
			$this->facade->smarty->assign('ips',$out);
			$this->facade->smarty->assign('group',$g->assoc());
			$this->facade->smarty->assign('anon_ips',$anon_memb);

		break;
		case 'remove':
			if ($id == $anon->_id) {
				$this->fail("$action: cant remove objects from ". ANONYMOUS_GROUP);
				break;
			}
			switch($subaction) {
			case 'try':
				$ip = new OBJ_IP($subid);
				$g = new OBJ_GROUP($id);
				$this->facade->smarty->assign('group',$g->assoc());
				$this->facade->smarty->assign('ip',$ip->assoc());
			break;
			case 'commit':
				$ip = new OBJ_IP($subid);
				if ($ip->move_to_gid($anon->_id))
					$this->success($action);
				else
					$this->fail($action);
			break;
			}
		break;
		case 'add':
			$ip = new OBJ_IP($subid);
			if ($ip->move_to_gid($id))
				$this->success("inserted object {$ip->_id} to group $id");
			else 
				$this->fail("insert failed");
		break;
		}
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
