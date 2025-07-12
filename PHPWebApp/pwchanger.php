<?php

require('_config.php');

function PwChanger_DoDebug()
{
	$Cmd = 'PWChangeApp contoso.com SomeUser@contoso.com OldPass NewPass';
	//print( $Cmd );
	exec( $Cmd , $Output , $Result );
	print( 'Result of '.$Cmd.' = '.$Result.'<br />' );
	foreach( $Output as $Line )
	{
		print( '<p>'.$Line.'</p>' );
	}
	
	$HadError = (0 != $Result);
	
	if( $HadError )
	{
		print( 'Failed<br/>' );
	}
	else
	{
		print( 'Succeeded<br/>' );
	}
}

function PwChanger_IsSSL() 
{
	if ( isset($_SERVER['HTTPS']) ) 
	{
		if ( 'on' == strtolower($_SERVER['HTTPS']) )
		{
			return true;
		}
		
		if ( '1' == $_SERVER['HTTPS'] )
		{
			return true;
		}
	} 
	elseif ( isset($_SERVER['SERVER_PORT']) && ( '443' == $_SERVER['SERVER_PORT'] ) ) 
	{
		return true;
	}
	
	return false;
}

function PwChanger_IsLoginValid( $Username , $Password )
{	
	global $PWCHANGER_DOMAIN_NAME;

	$HadError = false;
	
	$Cmd = sprintf( 'PWChangeApp ValidatePW "%s" "%s" "%s"' , $PWCHANGER_DOMAIN_NAME , $Username , $Password );
	//print( $Cmd );
	exec( $Cmd , $Output , $Result );
	foreach( $Output as $Line )
	{
		print( '<p>'.$Line.'</p>' );
	}
	
	$HadError = (0 != $Result);

	if( $HadError )
	{
		print( '<p style="color:red;">The system failed to validate the password, it may have been incorrect.<p/>' );
	}

	return !$HadError;
}

function PwChanger_ChangePassword( $Username , $Password , $NewPassword )
{
	global $PWCHANGER_DOMAIN_NAME;
	
	$HadError = false;
	
	$Cmd = sprintf( 'PWChangeApp ChangePW "%s" "%s" "%s" "%s"' , $PWCHANGER_DOMAIN_NAME , $Username , $Password , $NewPassword );
	//print( $Cmd );
	exec( $Cmd , $Output , $Result );
	foreach( $Output as $Line )
	{
		print( '<p>'.$Line.'</p>' );
	}
	
	$HadError = (0 != $Result);
	
	if( !$HadError )
	{
		//Becuase of the funkiness to change the password, we're going to check
		//if the passowrd actually changed:
		$HadError = !PwChanger_IsLoginValid( $Username , $NewPassword );
		if( $HadError )
		{
			print( '<p style="color:red;">Password not changed.<p/>' );
		}
	}

	if( $HadError )
	{
		print( '<p style="color:red;">The system failed to change your password, complexity requirements may have not been met.<p/>' );
		//print( '<p style="color:red;">The result was: '.$Result.'<p/>' );
	}
	return !$HadError;
}

?>