$(function () {
    $("#refreshBtn").click(function () {

        $(this).attr("disabled", "disabled");

        $.ajax({
            type: "POST",
            url: "/Web/devices.xml?refresh=true&type=urn:schemas-upnp-org:device:MediaRenderer:1",
            dataType: "xml",
            success: function (xml) {

                $("#refreshBtn").removeAttr("disabled");
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

    $("#volPBtn").click(function () {
        $.post("/web/control?action=volp&device=" + $("#devices").val());
    });

    $("#volMBtn").click(function () {
        $.post("/web/control?action=volm&device=" + $("#devices").val());
    });

    $("#playBtn").click(function () {
        $.post("/web/control?action=play&device=" + $("#devices").val() + "&id=" + $("#idInput").val());
    });

    $("#stopBtn").click(function () {
        $.post("/web/control?action=stop&device=" + $("#devices").val());
    });
});