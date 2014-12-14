function SetNewPositionText(position) {
    var newPos = new Date(0, 0, 0, 0, 0, position);

    $("#streamPos").html(newPos.getHours() + ":" +
            (newPos.getMinutes() < 10 ? "0" + newPos.getMinutes() : newPos.getMinutes()) + ":" +
            (newPos.getSeconds() < 10 ? "0" + newPos.getSeconds() : newPos.getSeconds()));
};

function SetNewPosition(position) {
    if (document.streamBasePosition) {
        SetNewPositionText(document.streamBasePosition + position);
        $("#seekSlider").slider("value", document.streamBasePosition + position);
    }
    else {
        SetNewPositionText(position);
        $("#seekSlider").slider("value", position);
    }
}

function SetNewVolume(volume) {
    $("#volumeSlider").slider("value", volume);
    $("#volumeVal").html(Math.round(volume * 100) + "%");
}

function UpdateTimeUrl(url, starttime) {
    var timeIndex = url.search(/&starttime=/i);
    if (timeIndex >= 0)
        url = url.substr(0, timeIndex);

    return url + "&starttime=" + starttime;
}

function RefreshUrl(starttime) {
    var src = UpdateTimeUrl($("#videoLink").prop("href"), starttime);

    $("#videoLink").prop("href", src);
}

$(function () {

    document.delayTimer = new Object();
    var splitArray = $("#streamLength").text().split(":");
    var streamLength = (new Number(splitArray[0]) * 3600) + (new Number(splitArray[1]) * 60) + new Number(splitArray[2]);

    $("#seekSlider").slider({
        min: 0,
        max: streamLength,
        value: 0,
        slide: function (event, ui) {
            var uiVal = ui.value;

            document.streamBasePosition = uiVal;
            SetNewPositionText(uiVal);

            RefreshUrl(uiVal);

            clearTimeout(document.delayTimer);
            document.delayTimer = setTimeout(function () {
                if ($("#streamVideo").get(0)) {
                    var url = $("#streamVideo").get(0).currentSrc;

                    $("#streamVideo").get(0).pause();
                    $("#streamVideo").prop("src", UpdateTimeUrl(url, uiVal));
                    $("#streamVideo").get(0).load();
                    $("#streamVideo").get(0).play();
                }
                else if ($("#silverlightControlHost").get(0)) {
                    var url = $("#videoLink").prop("href");

                    if ($("#silverlightControlHost").get(0).Content)
                        $("#silverlightControlHost").get(0).Content.Player.Play(url);
                }
                else {
                    var url = $("#videoLink").prop("href");

                    $f("videoLink").play(url.replace("=", "%3D").replace("&", "%26"));
                }
            }, 300);
        }
    });

    $("#volumeSlider").slider({
        min: 0,
        max: 1,
        value: 1,
        step: 0.01,
        slide: function (event, ui) {
            if ($("#streamVideo").get(0))
                $("#streamVideo").prop("volume", ui.value);
            else if ($("#silverlightControlHost").get(0))
                $("#silverlightControlHost").get(0).Content.Player.SetVolume(ui.value);
            else
                $f("videoLink").setVolume(ui.value * 100);

            $("#volumeVal").html(Math.round(ui.value * 100) + "%");
        }
    });

    $("#streamVideo").bind("timeupdate", function () {
        var currenttime = new Number($("#streamVideo").prop("currentTime"));
        SetNewPosition(currenttime);
    });

    $("#playButton").button({ icons: { primary: "ui-icon-play" }, text: false });
    $("#playButton").click(function () {
        if ($("#streamVideo").get(0))
            $("#streamVideo").get(0).play();
        else if ($("#silverlightControlHost").get(0))
            $("#silverlightControlHost").get(0).Content.Player.Play($("#videoLink").prop("href"));
        else
            $f("videoLink").play();
    });

    $("#pauseButton").button({ icons: { primary: "ui-icon-pause" }, text: false });
    $("#pauseButton").click(function () {
        if ($("#streamVideo").get(0))
            $("#streamVideo").get(0).pause();
        else if ($("#silverlightControlHost").get(0))
            $("#silverlightControlHost").get(0).Content.Player.Pause();
        else
            $f("videoLink").pause();
    });

    var submitForm = function () {
        $("#mainForm").submit();
    }

    $("#codecRadios").buttonset();
    $("#codecRadios").change(submitForm);

    if ($("#resDlg").is("div")) {
        $("#resDlg").dialog({
            autoOpen: false,
            height: 165,
            width: 250,
            modal: true,
            close: function () {
                var lastCheck = $.data($("#resolutionRadios").get(0), "lastCheck");
                $("#" + lastCheck).prop("checked", true);
                $("#resolutionRadios").buttonset("refresh");

                $(this).dialog("close");
            }
        });

        $("#resDlgButton").button().click(function () {
            var width = $("#resDlgWidth").val();
            var height = $("#resDlgHeight").val();
            $("#r0x0Radio").val(width + "x" + height);

            submitForm();
        });

        $("#resolutionRadios").buttonset();
        $("#resolutionRadios").change(function () {
            if ($("#r0x0Radio").is(":checked"))
                return;

            submitForm();
        });
        $.data($("#resolutionRadios").get(0), "lastCheck", $("#resolutionRadios input:checked").prop("id"));

        $("#r0x0Radio").button({ icons: { primary: "ui-icon-gear" }, text: false }).click(function () {
            $("#resDlg").dialog("open");
        });
    }

    $("#qualityRadios").buttonset();
    $("#qualityRadios").change(submitForm);

    $("#fullScreenButton").button({ icons: { primary: "ui-icon-arrow-4-diag" }, text: false });
    $("#fullScreenButton").click(function () {
        if ($("#streamVideo").get(0)) {
            var videoElement = $("#streamVideo").get(0);

            if (videoElement.requestFullscreen)
                videoElement.requestFullscreen();
            else if (videoElement.msRequestFullscreen)
                videoElement.msRequestFullscreen();
            else if (videoElement.mozRequestFullScreen)
                videoElement.mozRequestFullScreen();
            else if (videoElement.webkitRequestFullScreen)
                videoElement.webkitRequestFullScreen();
        }
    });

    if ($("#streamVideo").get(0)) {
        SetNewVolume($("#streamVideo").prop("volume"));

        //Nepodporovany autoplay - zobraz controls
        $("#streamVideo").get(0).addEventListener("stalled", function () {
            $("#streamVideo").get(0).controls = true;
        });

        $("#streamVideo").get(0).addEventListener("playing", function () {
            $("#streamVideo").get(0).controls = false;
        });
    }
    else if ($("#silverlightControlHost").get(0)) {
    }
    else {
        $f("videoLink", "/web/flowplayer.swf");

        $f("videoLink").onLoad(function () {
            SetNewVolume($f("videoLink").getVolume() / 100.0);
        });

        $f("videoLink").getCommonClip().onStart(function () {
            document.posInterval = setInterval(function () {
                SetNewPosition($f("videoLink").getTime());
            }, 1000);
        });

        $f("videoLink").getCommonClip().onStop(function () {
            clearInterval(document.posInterval)
        });

        $f("videoLink").play();
    }
});

function WebPlayerPositionChange(totalSeconds) {
    SetNewPosition(totalSeconds);
}

function WebPlayerSizeChange(width, height) {
    $("#silverlightControlHost").css("width", width);
    $("#silverlightControlHost").css("height", height);
}

function silverlightControlLoaded(sender, args) {
    var url = $("#videoLink").prop("href");
    var silverlight = $("#silverlightControlHost").get(0);

    SetNewVolume(silverlight.Content.Player.GetVolume());

    silverlight.Content.Player.Play(url);
}

function onSilverlightError(sender, args) {
    var appSource = "";
    if (sender != null && sender != 0) {
        appSource = sender.getHost().Source;
    }

    var errorType = args.ErrorType;
    var iErrorCode = args.ErrorCode;

    if (errorType == "ImageError" || errorType == "MediaError") {
        return;
    }

    var errMsg = "Unhandled Error in Silverlight Application " + appSource + "\n";

    errMsg += "Code: " + iErrorCode + "    \n";
    errMsg += "Category: " + errorType + "       \n";
    errMsg += "Message: " + args.ErrorMessage + "     \n";

    if (errorType == "ParserError") {
        errMsg += "File: " + args.xamlFile + "     \n";
        errMsg += "Line: " + args.lineNumber + "     \n";
        errMsg += "Position: " + args.charPosition + "     \n";
    }
    else if (errorType == "RuntimeError") {
        if (args.lineNumber != 0) {
            errMsg += "Line: " + args.lineNumber + "     \n";
            errMsg += "Position: " + args.charPosition + "     \n";
        }
        errMsg += "MethodName: " + args.methodName + "     \n";
    }

    throw new Error(errMsg);
}
