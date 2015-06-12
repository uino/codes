<?php


//----------------------------------------------------

function add_to_log($s) {
   $logfile = 'log.txt';
   $m = file_get_contents($logfile);
   file_put_contents($logfile, $s.$m);
}

//----------------------------------------------------

if (! empty($_GET['log'])) {
   $msg = $_GET['log'];
   add_to_log($msg."\n");
}  

if (! empty($_GET['reset'])) {
   file_put_contents('log.txt', '');
}  

//----------------------------------------------------

// demo: file_put_contents('log.txt', "x\n", FILE_APPEND);

//----------------------------------------------------

$autorefresh = ! empty($_GET['refresh']);
if ($autorefresh) {
   $page = $_SERVER['PHP_SELF']."?refresh=1";
   $delay= "1";
   header("Refresh: $delay; url=$page");
} 


//----------------------------------------------------

?>

<!DOCTYPE html>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />
<html>
<body>
<style>
div {
   margin-top: 1em;
}
</style>

<div>Query: <?php echo print_r($_GET, true); ?></div>


<div>
<form action='index.php' name='form' method='GET'>
<div><input type='submit' name='reset' value='Reset log' /></div>
<input type='submit' name='log' value='Test' /></div>
</form>
</div>

<?php 
   if ($autorefresh) {
      echo "<div><a href='index.php'>Stop automatic refresh</a></div>";
   } else {
      echo "<div><a href='index.php?refresh=1'>Start automatic refresh</a></div>";
   }
?>

<div>
<textarea rows=30 cols=100 style="overflow-y: scroll">
<?php echo file_get_contents('log.txt'); ?>
</textarea>

</div>
</body>
</html>

