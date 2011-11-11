{include file='../../../shared/header.tpl'}
<div id="middle">
{foreach $groups as $group}
{strip}
<a href={$site}/group/edit/{$group._id}/try>[edit]</a> {$group._descr} [<a href="{$site}/group/members/{$group._id}/view">members</a>] [<a href={$site}/group/delete/{$group._id}/try>del</a>]<br>
{/strip}
{/foreach}
</div>
{include file='../../../shared/footer.tpl'}