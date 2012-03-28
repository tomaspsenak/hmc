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

function RefreshUrl(starttime) {
    var oldSrc = $("#videoLink").prop("href");
    var timeIndex = oldSrc.search(/&starttime=/i);

    if (timeIndex >= 0)
        oldSrc = oldSrc.substr(0, timeIndex);

    $("#videoLink").prop("href", oldSrc + "&starttime=" + starttime);
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
            document.streamBasePosition = ui.value;
            SetNewPositionText(ui.value);

            RefreshUrl(ui.value);

            clearTimeout(document.delayTimer);
            document.delayTimer = setTimeout(function () {
                if ($("#streamVideo").get(0)) {
                    var url = $("#videoLink").prop("href");

                    $("#streamVideo").get(0).pause();
                    $("#streamVideo").prop("src", url);
                    $("#streamVideo").get(0).load();
                    $("#streamVideo").get(0).play();
                }
                else {
                    var url = $("#videoLink").prop("href");
                    if ($("#silverlightControlHost").get(0).Content)
                        $("#silverlightControlHost").get(0).Content.Player.Play(url);
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
            else
                $("#silverlightControlHost").get(0).Content.Player.SetVolume(ui.value);

            $("#volumeVal").html(Math.round(ui.value * 100) + "%");
        }
    });

    $("#streamVideo").bind("timeupdate", function () {
        var currenttime = new Number($("#streamVideo").prop("currentTime"));
        SetNewPosition(currenttime);
    });

    $("#playButton").button();
    $("#playButton").css("background-color", "#416271");
    $("#playButton").css("color", "#ffffff");
    $("#playButton").click(function () {
        if ($("#streamVideo").get(0))
            $("#streamVideo").get(0).play();
        else
            $("#silverlightControlHost").get(0).Content.Player.Play($("#videoLink").prop("href"));
    });

    $("#pauseButton").button();
    $("#pauseButton").click(function () {
        if ($("#streamVideo").get(0))
            $("#streamVideo").get(0).pause();
        else
            $("#silverlightControlHost").get(0).Content.Player.Pause();
    });

    var submitForm = function () {
        $("#mainForm").submit();
    }

    $("#codecRadios").buttonset();
    $("#codecRadios").change(submitForm);

    $("#resolutionRadios").buttonset();
    $("#resolutionRadios").change(submitForm);

    $("#qualityRadios").buttonset();
    $("#qualityRadios").change(submitForm);

    $("#submitButton").button();
    $("#submitButton").css("background-color", "#416271");
    $("#submitButton").css("color", "#ffffff");

    if ($("#streamVideo").get(0))
        SetNewVolume($("#streamVideo").prop("volume"));
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
