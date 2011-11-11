{include file='../../../shared/header.tpl'}
<div id="middle">
	<form method=POST action="{$site}/auth/login">
	u: <input type=text name=user><br>
	p: <input type=password name=pass><br>
	<input type=submit name=GO value=GO>
	<form>
</div>
{include file='../../../shared/footer.tpl'}