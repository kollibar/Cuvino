window.onload = function() {
    var els = document.getElementsByTagName("blockquote");

    for(var e = 0; e < els.length; e++) {
        var bq = els[e];
        var bqp = bq.getElementsByTagName("p")[0];
        var text = bqp.innerHTML;
        var newText = text;
        
        if (text.indexOf("warning") == 0) {
            bq.style.borderLeft = "4px solid #FE9A2E";
            newText = text.substr("warning".length);
        }
        else if (text.indexOf("important") == 0) {
            bq.style.borderLeft = "4px solid #FE2E2E";
            newText = text.substr("important".length);
        }
        else {
            bq.style.borderLeft = "4px solid #A4A4A4";
        }
        
        bqp.style.fontSize = "0.9em";
        bqp.style.fontStyle = "italic";
        bqp.style.backgroundColor = "transparent";
       
        bq.style.backgroundColor = "transparent";
        bq.style.marginBottom = "8px";
        
        bqp.innerHTML = newText.trim();
    }    
    
    //paragraphs
    if (document.querySelectorAll) {    
        el = document.querySelectorAll("[role=main]")[0];
        el.style.fontFamily = "calibri, times new roman";
        el.style.fontSize = "1.1em";
    }   

    //cells
    els = document.getElementsByTagName("td");
    for(var e = 0; e < els.length; e++) {
        var cell = els[e];  

        cell.style.paddingLeft = "5px";
        cell.style.paddingRight = "5px";
        cell.style.border = "solid 1px #aaa";
        cell.style.textAlign = "center";

        if (cell.textContent.toLowerCase().indexOf("low") >= 0 || 
            cell.textContent.toLowerCase().indexOf("no") >= 0 )
        {
            cell.style.backgroundColor = "#faa";
        }
        else if (cell.textContent.toLowerCase().indexOf("high") >= 0 || 
            cell.textContent.toLowerCase().indexOf("yes") >= 0 ||
            cell.textContent.toLowerCase().indexOf("immediate") >= 0 )
        {
            cell.style.backgroundColor = "#afa";
        }
        else if (cell.textContent.toLowerCase().indexOf("hardly") >= 0 || 
            cell.textContent.toLowerCase().indexOf("medium") >= 0 ||
            cell.textContent.toLowerCase().indexOf("average") >= 0 ||
            cell.textContent.toLowerCase().indexOf("depends") >= 0 )
        {
            cell.style.backgroundColor = "#ffc";
        }
    }
    els = document.getElementsByTagName("th");
    for(var e = 0; e < els.length; e++) { 
        var cell = els[e];  
        cell.style.paddingLeft = "5px";
        cell.style.border = "solid 1px #aaa";
        cell.style.textAlign = "center";
    }
    
    //images
    window.onresize = function(event) {
        els = document.getElementsByTagName("img");

        for(var e = 0; e < els.length; e++) {
            var img = els[e];
            var oldWidth = img.getAttribute("oldWith");
            if (oldWidth) {
                img.width = parseInt(oldWidth);;
            }
            
            if (img.offsetWidth > img.parentNode.parentNode.offsetWidth) {
                img.setAttribute("oldWith", img.offsetWidth);
                img.width = img.parentNode.parentNode.offsetWidth;
            } 
        }
    };
    
    window.onresize();
    
}


document.body.style.backgroundImage = "url('../../grid.png')";