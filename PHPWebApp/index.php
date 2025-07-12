<?php
require( '_config.php' );
require( 'pwchanger.php' );

function Index_Init()
{
	
}

function Index_ShowContent_ShowChangePage()
{
	if( PwChanger_IsSSL() ) 
	{
?>
<form method="post">
Username:<br>
<input type="text" name="uname" value=""><br>
Current Password:<br>
<input type="password" name="cpwd" value=""><br>
New Password:<br>
<input type="password" name="npwd" value=""><br>
Confirm New Password:<br>
<input type="password" name="cnpwd" value=""><br>
<input type="submit" value="Submit">
<input type="hidden" name="stage" value="commit"/>
</form>
<?php
	} 
	else 
	{ 
		print( 'Change passwords requires an SSL connection. Please visit the secure version of this site</a>.<br/>' );
	}
}

function Index_ShowContent_ShowCommitPage()
{
	$HadError = false;
	
	print( 'Verifying current credentials ...<br/>' );
	if( PwChanger_IsLoginValid( $_POST['uname'] , $_POST['cpwd'] ) )
	{
		print( 'Credentials verified, changing password...<br/>' );
		if( strlen( $_POST['npwd'] ) < 1 )
		{
			print( '<p style="color:red;">The password provided is too short.<p/>' );
			$HadError = true;
		}
		
		if( $_POST['npwd'] != $_POST['cnpwd'] )
		{
			print( '<p style="color:red;">The passwords do not match.<p/>' );
			$HadError = true;
		}
		
		if( !$HadError )
		{
			$WasChanged = PwChanger_ChangePassword( $_POST['uname'] , $_POST['cpwd'] , $_POST['npwd'] );
			$HadError = !$WasChanged;
		}
	}
	else
	{
		$HadError = true;
		print( '<p style="color:red;">The credentials you entered were invalid, please log in with the correct credentials.<p/>' );
	}
		
	if( $HadError )
	{
		Index_ShowContent_ShowChangePage();
	}
	else
	{
		print( '<p style="color:green;">The password was successfully updated.<p/>' );
	}
}

function Index_ShowContent()
{
	if( isset($_POST['stage']) && $_POST['stage'] == 'commit' )
	{
		Index_ShowContent_ShowCommitPage();
	}
	else
	{
		Index_ShowContent_ShowChangePage();
	}
	
	print( '<p><a href="/">Return Home</a></p>' );
}

function Index_ShowDebug()
{
	PwChanger_DoDebug();
	print( '<p><a href="/">Return Home</a></p>' );
}
?>

<?php Index_Init(); ?>
<!DOCTYPE html>
<html>
<head>
<style>
body 
{
	background-color:gray;
}

h1
{
	color:lightblue;
}

div#main, div#footer
{
	color:orange;
	background-color:black;
	width:800px;
	margin:20px auto;
	padding:10px 20px;
}

div#footer
{
	width:600px;
	font-size:60%;
}

div#main a
{
	color:#F00;
	text-decoration:none;
}

div#main a:hover
{
	text-decoration:underline;
}

div#main a:visited
{
	color:#D00;
}
</style>
<title><?php print($PWCHANGER_COMPANY_NAME);?> Password Change</title>
</head>
<body>
<div id="main">
<h1><?php print($PWCHANGER_COMPANY_NAME);?></h1>
<h2>Password Change</h2>
<?php 
if( 0 )
{
	Index_ShowDebug();
	Index_ShowContent();
}
else
{
	Index_ShowContent();
}

?>
</div>
<div id="footer">
&copy; <?php print(date( "Y" ).' '.$PWCHANGER_COMPANY_NAME);?>
</div>
</body>
</html>