<?php 
define('ROOT',realpath(dirname(__FILE__)) . '/../');
define('SITE',preg_replace("/\/index.php/","",$_SERVER{'SCRIPT_NAME'})); 
define('SHARED',ROOT . "/app/shared/");
define('SESSION_EXPIRE',300); /* in seconds */
define('DEBUG',1); /* $this->warn honors this */
define('SSL_ONLY',0); /* die if not ssl */
define('MIN_GROUP_ID',1000000000); /* used by OBJ_OPTION to determine owner kind */
define('ANONYMOUS_GROUP',"ANONYMOUS GROUP");
define('MIN_GROUP_DESCRIPTION',5); /* groups with name less then 5 letters can not be created */
define('DEFAULT_FACTORY','group'); /* first thing user sees after login */
define('ENCRYPTED_PASSWORDS',1); /* keep paswords sha1 hashed inside the database */
define('FAIL', -1);
define('SUCCESS',1);
define('UNDEFINED',0);
if (SSL_ONLY == 1 && !isset($_SERVER{'HTTPS'})) 
	die('use ssl, or edit index.php and set SSL_ONLY to 0');

include(ROOT . '/obj/err.php'); /* xxx: not the best name */
include(ROOT . '/obj/db.php');
include(ROOT . '/obj/user.php');
include(ROOT . '/obj/ip.php');
include(ROOT . '/obj/group.php');
include(ROOT . '/obj/option.php');
include(ROOT . '/app/factory/factory.php');
include(ROOT . '/app/primitive.php');
include(ROOT . '/lib/smarty/Smarty.class.php');
session_start();
$f = new FACTORY($_GET);
$f->start();
$f->render();
	
function home() {
	header('Location: ' . SITE);
	exit(0);
}
?>