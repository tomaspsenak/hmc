function TimeToValue(str) {
    var splitArray = str.split(":");
    return (new Number(splitArray[0]) * 3600) + (new Number(splitArray[1]) * 60) + new Number(splitArray[2]);
}

$(function () {

    $(document).tooltip();

    $("#refreshBtn").button().click(function () {

        $(this).hide();
        $("#loadingImg").show();

        $.ajax({
            type: "POST",
            url: "/Web/devices.xml?refresh=true&type=urn:schemas-upnp-org:device:MediaRenderer:1",
            dataType: "xml",
            success: function (xml) {

                $("#refreshBtn").show();
                $("#loadingImg").hide();
                $("#devices").empty();

                $(xml).find("option").each(function () {

                    $("#devices").append($("<option/>", {
                        value: $(this).attr("value"),
                        text: $(this).text()
                    }));

                });
            }
        });
    });

    $("#volPBtn").button();
    $("#volMBtn").button();
    $("#playBtn").button();
    $("#stopBtn").button();

    var streamLength = TimeToValue($("#streamLength").text());

    var streamValue = TimeToValue($("#posInput").val());

    $("#seekSlider").slider({
        min: 0,
        max: streamLength,
        value: streamValue,
        slide: function (event, ui) {
            var newPos = new Date(0, 0, 0, 0, 0, ui.value);
            var newPosStr = newPos.getHours() + ":" +
                (newPos.getMinutes() < 10 ? "0" + newPos.getMinutes() : newPos.getMinutes()) + ":" +
                (newPos.getSeconds() < 10 ? "0" + newPos.getSeconds() : newPos.getSeconds());

            $("#streamPos").html(newPosStr);
            $("#posInput").val(newPosStr);
        }
    });
});