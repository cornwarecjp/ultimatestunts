<?php
$datafile = "metaserver.dat";
$logfile = "metaserver.log";

function addLogEntry($text)
{
	global $logfile;
	$fp = fopen($logfile, "a");
	fwrite($fp, $text);
	fclose($fp);
}

$timenow = time();

//Read data
$serverdatasize = filesize($datafile);
if($serverdatasize > 0)
{
	$fp = fopen($datafile, "r");
	$serverdata = fread($fp, $serverdatasize);
	fclose($fp);

	$serverdata = explode("\n\n", $serverdata);
	array_pop($serverdata); //Remove the last empty element
}
else
{
	$serverdata = array();
}

foreach($serverdata as $s=>$server)
{
	$info = explode("\n", $server);
	$serverdata[$s] = array(
		"version" => $info[0],
		"name" => $info[1],
		"ipnum" => $info[2],
		"port" => $info[3],
		"timestamp" => $info[4]
		);
}

$changed = false;

//Process setServer requests
if($_POST["setServer"] != "")
{
	$version = stripslashes($_POST["version"]);
	$name = stripslashes($_POST["name"]);
	$ipnum = getenv("REMOTE_ADDR");
	$port = stripslashes($_POST["port"]);

	echo "Setting server:\n";
	echo "Version = $version\n";
	echo "Name = $name\n";
	echo "IP = $ipnum\n";
	echo "Port = $port\n";
	echo "Timestamp = $timenow\n";

	addLogEntry(
		"setServer\n".
		"  Version = $version\n".
		"  Name = $name\n".
		"  IP = $ipnum\n".
		"  Port = $port\n".
		"  Timestamp = $timenow\n".
		"  Browser = " . getenv("HTTP_USER_AGENT") . "\n"
		);

	$setserver = array(
		"version" => $version,
		"name" => $name,
		"ipnum" => $ipnum,
		"port" => $port,
		"timestamp" => $timenow
		);


	$found = false;
	foreach($serverdata as $s=>$server)
		if($server["ipnum"] == $ipnum && $server["port"] == $port)
		{
			$serverdata[$s] = $setserver;
			$found = true;
		}

	if(!$found)
		$serverdata[] = $setserver;

	$changed = true;
}

//Process removeServer requests
else if($_POST["removeServer"] != "")
{
	$ipnum = getenv("REMOTE_ADDR");
	$port = stripslashes($_POST["port"]);

	addLogEntry(
		"removeServer\n".
		"  IP = $ipnum\n".
		"  port = $port\n".
		"  Browser = " . getenv("HTTP_USER_AGENT") . "\n"
		);

	//Remove all matching entries:
	$newdata = array();
	foreach($serverdata as $server)
		if($server["ipnum"] != $ipnum || $server["port"] != $port)
			$newdata[] = $server;
	$serverdata = $newdata;

	$changed = true;
}

//If there is no recognizable POST command
else
{
	addLogEntry(
		"Page loaded without POST commands\n".
		"  Browser = " . getenv("HTTP_USER_AGENT") . "\n"
		);
}


//Remove servers with too old timestamp
$newdata = array();
foreach($serverdata as $server)
	if($timenow - $server["timestamp"] < 3600) //one hour since last update
	{
		$newdata[] = $server;
	}
	else
	{
		$changed = true; //this item is removed
	}
$serverdata = $newdata;

//Write data
if($changed)
{
	$fp = fopen($datafile, "w");

	foreach($serverdata as $server)
	{
		fwrite($fp, $server["version"]."\n");
		fwrite($fp, $server["name"]."\n");
		fwrite($fp, $server["ipnum"]."\n");
		fwrite($fp, $server["port"]."\n");
		fwrite($fp, $server["timestamp"]."\n");
		fwrite($fp, "\n");
	}

	fclose($fp);
}

?>
<html>
<h1>Ultimate Stunts metaserver interface</h1>
The purpose of this page is to connect Ultimate Stunts players
over the internet with each other. This works as follows:
<ul>
	<li>Server programs post their data on this page</li>
	<li>Client programs read the server list from this page</li>
</ul>
Client programs can not really read HTML: instead, they ignore everything
until a single line with the text "START" is found. Then, they read and
parse the server list, until a line containing "END" is found.
<p>
The following section, containing the server list, is used by the client
program:
<pre>
START
<?php

$serverdatasize = filesize($datafile);
if($serverdatasize > 0)
{
	$fp = fopen($datafile, "r");
	echo fread($fp, $serverdatasize);
	fclose($fp);
}

//include($datafile);

?>
END
</pre>

For each server, the first line is a server version identifier string. The meaning of the following
lines depend on the server version, but an empty line is always the end of the server information.
The meaning of the lines is as follows, for different versions:

<h3>ULTIMATESTUNTS 0.7.2</h3>
<ul>
	<li>Server name</li>
	<li>IP number</li>
	<li>UDP port number</li>
	<li>Last contact timestamp</li>
</ul>

</html>

