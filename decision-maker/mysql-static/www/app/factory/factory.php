<?php 
/* IO FACTORY */
class FACTORY {
	public $smarty;
	public $factory;
	public $factory_name;
	public $method;
	public $params;
	public $factory_producer;
	public $factory_root;
	function __construct($g = array()) {
		$factory_name = DEFAULT_FACTORY; 
		$this->method = 'unknown';
		$params = array();
		$get = array();
		if (array_key_exists('q',$g)) {
			$get = explode('/',$g['q']);
		}
		$c = count($get);
		switch($c) {
		case ($c >= 2):
			$factory0 = array_shift($get);
			$method0 = array_shift($get);
			$this->params = $get;
			
			if ($this->is_word($factory0)) {	
				$factory_name = $factory0;
			}
			if ($this->is_word($method0)) {
				$this->method = $method0;
			}
			break;
		case 1:
			if ($this->is_word($get[0])) {	
				$factory_name = $get[0];
			}
			break;
		}
		/* overwrite the factory if we need to authenticate */
		$need_auth = OBJ_USER::need_auth($factory_name);
		if ($need_auth) {
			$factory_name = 'auth';
			$this->method = ($this->method == 'logout') ? 'logout' : 'login';
		}
		if (!$this->is_word($factory_name)) {
			die('xxx'); /* never reached */
		}
		$this->factory_root = ROOT . "app/factory/$factory_name/";
		$this->factory_producer = "{$this->factory_root}/producer.php";
		if (file_exists($this->factory_producer)) {
			require_once($this->factory_producer);
		} else {
			home(); /* xxx: fall back to 'unknown' */
		}
		$this->factory_name = $factory_name;
		$this->smarty = new Smarty();
		$this->factory = new $factory_name($this);
		$this->smarty->assign('site',SITE);
		$this->smarty->assign('factory',$factory_name);
		$this->smarty->assign('method',$this->method);
		$this->smarty->assign('title',"/factory/$factory_name/");
		$this->smarty->assign('need_auth',$need_auth);
		$this->smarty->assign('submenu',$this->factory->submenu($factory_name));
		//echo "factory: $factory_name method: {$this->method}<br>";
	}
	function start() {
		$c = count($this->params);
		switch($c) {
		case ($c >= 4): /* factory/method/action/id/subaction/id */
			$id = intval(array_shift($this->params));
			$action = array_shift($this->params);
			$subid = intval(array_shift($this->params));
			$subaction = array_shift($this->params);
			$this->factory->{$this->method}($id,$action,$subid,$subaction);
		break;
		case 3:
			$id = intval(array_shift($this->params));
			$action = array_shift($this->params);
			$subid = intval(array_shift($this->params));
			$this->factory->{$this->method}($id,$action,$subid);
		break;
		case 2:
			$id = intval(array_shift($this->params));
			$action = array_shift($this->params);
			$this->factory->{$this->method}($id,$action);
		break;
		case 1:
			$id = intval(array_shift($this->params));
			$this->factory->{$this->method}($id);
			break;
		case 0:
			$this->factory->{$this->method}();			
			break;
		}
		$this->smarty->assign('message',$this->factory->message);
		if (isset($action))
			$this->smarty->assign('action',$action);
		if (isset($subaction))
			$this->smarty->assign('subaction',$subaction);
	}
	function render() {
		switch ($this->factory->rc) {
		case FAIL: 
			$template = SHARED . "/fail.tpl"; 
		break;
		case SUCCESS: 
			$template = SHARED . "/success.tpl"; 
		break;
		default:
			$template = "{$this->factory_root}/output/{$this->factory->template}";
		}
		$this->smarty->display($template);
	}
	function is_word($w,$len = 20) {
		return (strlen($w) <= $len && preg_match('/^[a-z]+$/',$w));
	}
}
?>