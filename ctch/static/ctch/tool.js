window.onload=function(){
	c=document.getElementById("tab1");
	c.addEventListener("click", click1);
	c=document.getElementById("tab2");
	c.addEventListener("click", click2);
	raw=document.getElementById('ugly');
	raw.addEventListener("change",modify);
	current=document.getElementById("file_name");
	current.addEventListener("click",attach);
	btn=document.getElementById("pretty");
	btn.addEventListener("click",attach);
}

function click1() {
	c1=document.getElementById("problem");
	c1.style.display="block";
	c=document.getElementById("tab1");
	c.setAttribute('class','current'); 
	c=document.getElementById("tab2");
	c.setAttribute('class','tab'); 
	c2=document.getElementById("file");
	c2.style.display="none";
	c=document.getElementById("tool_name");
	c.innerHTML="Just Try CTCH";
}
function click2() {
	c1=document.getElementById("problem");
	c1.style.display="none";
	c=document.getElementById("tab2");
	c.setAttribute('class','current'); 
	c=document.getElementById("tab1");
	c.setAttribute('class','tab'); 
	c2=document.getElementById("file");
	c2.style.display="block";
	c=document.getElementById("tool_name");
	c.innerHTML="Use CTCH in Practice";
}

function modify(){
	str=raw.value;
	str=str.split("\\");
	res=str[str.length-1]
	
	current=document.getElementById("file_name");
	current.value=res;
}

function attach(){
	raw.click();
}