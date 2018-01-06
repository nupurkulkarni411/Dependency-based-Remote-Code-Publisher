function OnClickDiv(divId, buttonId){
	var value = document.getElementById(divId).style.display;
	if(value != 'none'){
		document.getElementById(divId).style.display = 'none';
		document.getElementById(buttonId).innerHTML = '+';
	}else{
		document.getElementById(divId).style.display = 'block';
		document.getElementById(buttonId).innerHTML = '-';
	}
}
