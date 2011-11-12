{include file='../../../shared/header.tpl'}
<div id="middle">
<form method=POST action={$site}/group/edit/{$group._id}/commit>
<input type=text name=descr value='{$group._descr}'><br>
<input type=submit name=GO value=GO>
</form>
</div>
{include file='../../../shared/footer.tpl'}