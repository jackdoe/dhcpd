<?php
class ERR {
	public $warning;
	function err($msg) {
		die(get_class($this) . ": $msg");
	}
	
	function warn() {
		if (! DEBUG)
			return;
		$trace=debug_backtrace();
		$caller=$trace[1];
		
		$fx = $caller['function'];
		$file = basename($caller['file']);
		$line = $caller['line'];
		$fxargs=array();
		foreach($caller['args'] as $av) {
			if (is_string($av)) {
				array_push($fxargs,"'$av'");
			} elseif (is_array($av)) {
				array_push($fxargs,"array(".implode(array_values($av),",").")");
			} elseif (is_int($av) || is_bool($av) || is_float($av)) {
				array_push($fxargs,$av);
			} elseif (is_object($av)) {
				array_push($fxargs,get_class($av));
			} elseif (is_null($av)) {
				array_push($fxargs,'null');
			} else {
				array_push($fxargs,gettype($av));
			}
		}
		$args = func_get_args();
		$c = get_class($this);
		echo "<center><pre>";
		echo "$file:$line:$c->$fx(".implode($fxargs,",")."): ". implode($args,"\n");
		echo "</pre></center>";
	}
	function warn_bt($arg) {
		$this->trace($arg);
	}
	function trace($arg = "") {
		$debug = debug_backtrace();
		foreach ($debug as $d) {
			$obj = is_object($d{'object'}) ? get_class($d{'object'}) : "NOOBJ";
			$this->warn("$arg: {$d{'file'}}:{$d{'line'}}:class:$obj{$d{'type'}}{$d{'function'}}(" . implode(",",$d{'args'}) . ")");
		}
	}
	function valid_ip($ip) {
		if (ip2long($ip))
			return true;
		
		return false;
	}
	function valid_string($descr, $min_len = 0) {
		return (mb_strlen($descr) > $min_len); 
	}
	function valid_mac($mac) {
		return preg_match('/^[a-fA-F0-9]{2}:[a-fA-F0-9]{2}:[a-fA-F0-9]{2}:[a-fA-F0-9]{2}:[a-fA-F0-9]{2}:[a-fA-F0-9]{2}$/',$mac);
	}
	function valid_nonzero($int) {
		if (is_int($int) && $int > 0) {
			return true;
		}
		return false;
	}
	function valid_id($id) {
		if (!is_int($id) || $id < 1) 
			return false;
		
		switch(get_class($this)) {
		case "OBJ_GROUP":	   
			if ($id < MIN_GROUP_ID) 
				return false;
			
			break;
		case "OBJ_IP":
			if ($id >= MIN_GROUP_ID) 
				return false;
			
			break;
		}
		return true;
	}
}
?>