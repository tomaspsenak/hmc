function TimeToValue(str) {
    var splitArray = str.split(":");
    return (new Number(splitArray[0]) * 3600) + (new Number(splitArray[1]) * 60) + new Number(splitArray[2]);
}

$(function () {

    $(document).tooltip();

    $("#devices").selectmenu();

    $("#devicesWorkingBar").progressbar({ value: false }).hide();

    $("#refreshBtn").button({ icons: { primary: "ui-icon-refresh"} }).click(function () {

        $(this).button({ disabled: true });
        $("#devices").next().hide();
        $("#devicesWorkingBar").show();

        $.ajax({
            type: "POST",
            url: "/Web/devices.xml?refresh=true&type=urn:schemas-upnp-org:device:MediaRenderer:1",
            dataType: "xml",
            success: function (xml) {

                $("#refreshBtn").button({ disabled: false });
                $("#devices").next().show();
                $("#devicesWorkingBar").hide();
                $("#devices").empty();

                $(xml).find("option").each(function () {

                    $("#devices").append($("<option/>", {
                        value: $(this).attr("value"),
                        text: $(this).text()
                    }));

                });

                $("#devices").selectmenu("refresh");
            }
        });
    });

    $("#volPBtn").button({ icons: { primary: "ui-icon-volume-on"} });
    $("#volMBtn").button({ icons: { primary: "ui-icon-volume-off "} });
    $("#playBtn").button({ icons: { primary: "ui-icon-play"} });
    $("#stopBtn").button({ icons: { primary: "ui-icon-stop"} });

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