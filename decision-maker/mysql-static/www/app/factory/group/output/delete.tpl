{include file='../../../shared/header.tpl'}
<div id="middle">
{$message|default:""}
{if isset($group._id)}
	are you sure?<br>
	this will move all members of gid {$group._id} to 'anonymous group'?<br>
	<a href={$site}/group/delete/{$group._id}/commit>yes, delete the group</a>
{/if}
</div>
{include file='../../../shared/footer.tpl'}
