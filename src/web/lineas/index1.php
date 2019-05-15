<!DOCTYPE html>
<html lang="es">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Subir archivos al servidor</title>
<meta name ="author" content ="Norfi Carrodeguas">
<style type="text/css" media="screen">
body{font-size:1.2em;}
</style>
</head>
<body>
<form enctype='multipart/form-data' action='' method='post'>
<input name='uploadedfile' type='file'><br>
<input type='submit' value='Subir archivo'>
</form>
<?php
$target_path = "uploads/";
$target_path = $target_path . basename( $_FILES['uploadedfile']['name']);
if(move_uploaded_file($_FILES['uploadedfile']['tmp_name'], $target_path))
{
echo "<span style='color:green;'". basename( $_FILES['uploadedfile']['name']). " ha sido subido</span><br";
}else{
echo "Ha ocurrido un error, trate de nuevo!";
}
?>
</body>
</html>
