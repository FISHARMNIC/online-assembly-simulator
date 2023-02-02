window.linenum = 0;
window.oldMem = -1;

var runInterval;
var formattedcode;

document.getElementById("code").innerText =
    `mov %al, 0
inc %al
mov [%al], %al
cmp %al, 10
jne 1`;

function parse(ctn) {
    return ctn.split("\n")
}

function reset() {
    wasm_global.asm_memory_reset()
    linenum = 0;
    formattedcode = parse(document.getElementById("code").innerText);
    highlight()
    document.getElementById("runStatus").innerText = "Click Step to start"
    //oldMem = Array.from(memory.slice(0))
}


function nextLine() {

    if (linenum >= formattedcode.length) {
        //reset()
        document.getElementById("runStatus").innerText = "Halted";
    } else {
        document.getElementById("runStatus").innerText = "Running";
        highlight();
        run_c(formattedcode[linenum]);
        linenum++;
        highlight();
    }
}

function run_all()
{
    runInvterval = setInterval(nextLine, 500);
}

function stop_all()
{
    clearInterval(runInterval);
}

function highlight() {
    var hgcol = "lightblue"
    var hg = document.getElementById("highlight").innerHTML
    var og = document.getElementById("code").innerHTML
    if (linenum == 0) {
        document.getElementById("highlight").innerHTML = `<span style="background-color: ${hgcol};">` + og + "<br>"
        hg = document.getElementById("highlight").innerHTML
        document.getElementById("highlight").innerHTML = replace_nth(hg, "<br>", `</span><br>`, linenum + 1)
    } else {
        document.getElementById("highlight").innerHTML = replace_nth(og, "<br>", `<span style="background-color: ${hgcol};"><br>`, linenum)
        hg = document.getElementById("highlight").innerHTML
        document.getElementById("highlight").innerHTML = replace_nth(hg, "<br>", `</span><br>`, linenum + 1)
    }
    //document.getElementById("highlight").innerHTML = document.getElementById("highlight").innerHTML.replace(/mov/g, `<span style="text-color:red;">mov</span>`)
    updateMemory();
}
function replace_nth(s, f, r, n) {
    // From the given string s, find f, replace as r only on n’th occurrence
    return s.replace(RegExp("^(?:.*?" + f + "){" + n + "}"), x => x.replace(RegExp(f + "$"), r));
};