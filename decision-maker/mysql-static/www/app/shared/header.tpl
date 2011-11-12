<html>
<head>
<title>{$title|default:""}</title>
<style>
/*
	  css from: http://www.code-sucks.com
*/

	* { padding: 0; margin: 0; }
	
	body {
		font-family: Arial, Helvetica, sans-serif;
		font-size: 13px;
	}
	#wrapper { 
		margin: 0 auto;
		width: 922px;
	}
	#right { 
		color: #fff;
		border: 1px solid #ccc;
		background: #006666;
		margin: 10px 5px 5px 0px;
		padding: 10px;
		height: 500px;
		width: 190px;
		float: left;
	}
	#middle { 
		float: left;
		color: #333;
		border: 1px solid #ccc;
		background: #E9E9E9;
		margin: 10px 5px 5px 0px;
		padding: 10px;
		height: 500px;
		width: 456px;
		display: inline;
	}
	#middle > a {
		color: #555;
	}
	#middle > a:hover {
		color: #008584;
	}

	#left { 
		color: #fff;
		border: 1px solid #ccc;
		background: #008584;
		margin: 10px 5px 5px 0px;
		padding: 10px;
		height: 500px;
		width: 190px;
		float: left;
		position: relative;
		
	}
	
	#right > a {
		color: #fff;
	}
	#left > a {
		color: #fff;
	}
	
	#right > a:hover {
		color: #ddd;
	}
	#left > a:hover {
		color: #ddd;
	}
.clear { clear: both; }
</style>
</head>
<body>
<center>
	<font color=red>WARNING: this is still work in progress, do not use it in production</font><br>
</center>
<div id="wrapper">
	<div id="left">
		<h2>/{$factory}/</h2></h3><br>
		{foreach $submenu as $s}
		{strip}
		<a href="{$s.href}">s: {$s.text}</a><br>
		{/strip}
		{/foreach}
	</div>
