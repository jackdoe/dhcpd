{include file='../../../shared/header.tpl'}
<div id="middle">
	{foreach $ips as $ip}
	{strip}
	{$ip._id} {$ip._ip} {$ip._mac}
	{/strip}
	{/foreach}
</div>
{include file='../../../shared/footer.tpl'}