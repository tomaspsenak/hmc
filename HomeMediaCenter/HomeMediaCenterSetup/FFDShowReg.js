function RunScript() {
    var shell = new ActiveXObject("WScript.Shell");

    try { shell.RegWrite("HKEY_CURRENT_USER\\Software\\GNU\\ffdshow_dxva\\isWhitelist", 0, "REG_DWORD"); }
    catch (ex) { }

    try { shell.RegWrite("HKEY_CURRENT_USER\\Software\\GNU\\ffdshow\\isWhitelist", 0, "REG_DWORD"); }
    catch (ex) { }
}

RunScript();