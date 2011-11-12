{include file='../../../shared/header.tpl'}
<div id="middle">
{if $action == 'view'}
	<h3>{$group._descr}</h3><br>
	{foreach $ips as $ip}
	{strip}
	{$ip._id} {$ip._ip} {$ip._mac} <a href={$site}/group/members/{$group._id}/remove/{$ip._id}/try>remove</a><br>
	{/strip}
	{/foreach}
	{if count($anon_ips)}
		<br>
		<hr>
		<h3>ANONOYMOUS GROUP</h3><br>
		{foreach $anon_ips as $ip}
		{strip}
		{$ip._id} {$ip._ip} {$ip._mac} [<a href={$site}/group/members/{$group._id}/add/{$ip._id}>add</a>@{$group._descr}:<small>{$group._id}</small>]<br>
		{/strip}
		{/foreach}
	{/if}
{elseif $action == 'remove'} 
	{if $subaction == 'try'}
	are you sure you want to remove {$ip._id} from {$group._descr}?<br>
	<a href={$site}/group/members/{$group._id}/remove/{$ip._id}/commit>yes i am sure.</a>
	{/if}
{/if}
</div>
{include file='../../../shared/footer.tpl'}