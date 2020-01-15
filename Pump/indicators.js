uTank = {
    min_level: 0,
    canvasID: 'upper_tank',
    capacity: 0,
    labelID: "upper_volume",
    label: "Tanque Elevado",
    yc: 0,
    xc: 0,
    h: 0,
    w: 0,
    min: 0,
    restart: 0,
    height: 0,
    gap: 0,
}
lTank = {
    min_level: 0,
    canvasID: 'lower_tank',
    capacity: 0,
    labelID: "lower_volume",
    label: "Cisterna",
    yc: 0,
    xc: 0,
    h: 0,
    w: 0,
    min: 0,
    restart: 0,
    height: 0,
    gap: 0,
}
myPump = {
    state: 0,
    animationStage: 0,
    canvasID: 'pump',
    labelID: 'pump_state',
    Tmax: 0,
    startDelay: 0,
    ctx: null,
    yc: 0,
    xc: 0,
    h: 0,
    w: 0,
}

function drawUpperTankTemplate(tank) {
    let canvas = document.getElementById(tank.canvasID);
    if (canvas.getContext) {
        const xc = 0.5 * canvas.width;
        const yc = 0.5 * canvas.height;
        let h = 0.8 * canvas.height;
        let w = 0.7 * h;
        if (w > (0.95 * canvas.width)) {
            w = 0.95 * canvas.width;
            h = w / 0.7;
        }
        tank.h = h;
        tank.w = w;
        tank.xc = xc;
        tank.yc = yc;

        let ctx = canvas.getContext('2d');
        ctx.fillStyle = "#000000";
        ctx.strokeRect(xc - w / 2, 0.2 * h, w, h);
        ctx.fillRect(xc - w / 4, 0.1 * h, w / 2, 0.1 * h);
    }
}

function animatePump() {
    const x0 = myPump.xc - 0.32 * myPump.w;
    const w0 = 3 * myPump.w / 40;
    if (myPump.state == 1) {
        if (document.getElementById(myPump.labelID).innerHTML = "Apagada") {
            document.getElementById(myPump.labelID).innerHTML = "Encendida";
        }
        myPump.ctx.fillStyle = "#0000FF";
        switch (myPump.animationStage) {
            case 0:
                myPump.ctx.clearRect(x0 - 1, myPump.yc
                    - 13 * myPump.h / 40, w0 + 1, 20 * myPump.h / 40);
                myPump.animationStage = 1;
                break;
            case 1:
                myPump.ctx.fillRect(x0, myPump.yc
                    - 3 * myPump.h / 40, w0, 10 * myPump.h / 40);
                myPump.animationStage = 2;
                break;
            case 2:
                myPump.ctx.fillRect(x0, myPump.yc
                    - 13 * myPump.h / 40, w0, 10 * myPump.h / 40);
                myPump.animationStage = 0;
                break;
        }
    } else {
        if (document.getElementById(myPump.labelID).innerHTML = "Encendida") {
            document.getElementById(myPump.labelID).innerHTML = "Apagada";
        }
        myPump.ctx.clearRect(x0 - 1, myPump.yc
            - 13 * myPump.h / 40, w0 + 1, 20 * myPump.h / 40);
    }
    setTimeout(animatePump, 500);
}

function drawPumpTemplate(pump) {
    let canvas = document.getElementById(pump.canvasID);
    if (canvas.getContext) {
        const xc = 0.5 * canvas.width;
        const yc = 0.5 * canvas.height;
        let w = 0.8 * canvas.width;
        let h = w;
        if (h > (0.8 * canvas.height)) {
            h = 0.8 * canvas.height;
            w = h;
        }
        pump.h = h;
        pump.w = w;
        pump.xc = xc;
        pump.yc = yc;
        const pxl = w / 48;
        const x0 = xc - 0.5 * w;
        const y0 = 0.2 * h;

        let ctx = canvas.getContext('2d');
        pump.ctx = ctx;

        ctx.beginPath();
        ctx.moveTo(x0 + 5 * pxl, y0);
        ctx.lineTo(x0 + 8 * pxl, y0);
        ctx.lineTo(x0 + 8 * pxl, y0 + 14 * pxl);
        ctx.lineTo(x0 + 5 * pxl, y0 + 15 * pxl);
        ctx.lineTo(x0 + 5 * pxl, y0 + 30 * pxl);
        ctx.lineTo(x0 + 8 * pxl, y0 + 31 * pxl);
        ctx.lineTo(x0 + 13 * pxl, y0 + 31 * pxl);
        ctx.moveTo(x0 + 16 * pxl, y0);
        ctx.lineTo(x0 + 13 * pxl, y0);
        ctx.lineTo(x0 + 13 * pxl, y0);
        ctx.lineTo(x0 + 13 * pxl, y0 + 34 * pxl);
        ctx.lineTo(x0 + 16 * pxl, y0 + 34 * pxl);
        ctx.lineTo(x0 + 21 * pxl, y0 + 28 * pxl);
        ctx.lineTo(x0 + 24 * pxl, y0 + 28 * pxl);
        ctx.lineTo(x0 + 25 * pxl, y0 + 32 * pxl);
        ctx.lineTo(x0 + 46 * pxl, y0 + 32 * pxl);
        ctx.lineTo(x0 + 47 * pxl, y0 + 31 * pxl);
        ctx.lineTo(x0 + 47 * pxl, y0 + 15 * pxl);
        ctx.lineTo(x0 + 46 * pxl, y0 + 14 * pxl);
        ctx.lineTo(x0 + 25 * pxl, y0 + 14 * pxl);
        ctx.lineTo(x0 + 24 * pxl, y0 + 18 * pxl);
        ctx.lineTo(x0 + 21 * pxl, y0 + 18 * pxl);
        ctx.lineTo(x0 + 16 * pxl, y0 + 12 * pxl);
        ctx.lineTo(x0 + 13 * pxl, y0 + 12 * pxl);
        ctx.moveTo(x0 + 29 * pxl, y0 + 14 * pxl);
        ctx.lineTo(x0 + 29 * pxl, y0 + 28 * pxl);
        ctx.moveTo(x0 + 32 * pxl, y0 + 27 * pxl);
        ctx.lineTo(x0 + 41 * pxl, y0 + 27 * pxl);
        ctx.moveTo(x0 + 32 * pxl, y0 + 23 * pxl);
        ctx.lineTo(x0 + 41 * pxl, y0 + 23 * pxl);
        ctx.moveTo(x0 + 32 * pxl, y0 + 19 * pxl);
        ctx.lineTo(x0 + 41 * pxl, y0 + 19 * pxl);
        ctx.moveTo(x0 + 43 * pxl, y0 + 10 * pxl);
        ctx.lineTo(x0 + 43 * pxl, y0 + 8 * pxl);
        ctx.lineTo(x0 + 31 * pxl, y0 + 8 * pxl);
        ctx.lineTo(x0 + 29 * pxl, y0 + 10 * pxl);
        ctx.moveTo(x0 + 30 * pxl, y0 + 35 * pxl);
        ctx.lineTo(x0 + 26 * pxl, y0 + 38 * pxl);
        ctx.lineTo(x0 + 24 * pxl, y0 + 38 * pxl);
        ctx.lineTo(x0 + 24 * pxl, y0 + 40 * pxl);
        ctx.lineTo(x0 + 46 * pxl, y0 + 40 * pxl);
        ctx.lineTo(x0 + 46 * pxl, y0 + 38 * pxl);
        ctx.lineTo(x0 + 44 * pxl, y0 + 38 * pxl);
        ctx.lineTo(x0 + 40 * pxl, y0 + 35 * pxl);
        ctx.strokeRect(x0, y0 + 20 * pxl, 5 * pxl, 6 * pxl);
        ctx.stroke();
    }
}

function drawLowerTankTemplate(tank) {
    let canvas = document.getElementById(tank.canvasID);
    if (canvas.getContext) {
        const xc = 0.5 * canvas.width;
        const yc = 0.5 * canvas.height;
        let w = 0.8 * canvas.width;
        let h = 0.6 * w;
        if (h > (0.8 * canvas.height)) {
            h = 0.8 * canvas.height;
            w = h / 0.6;
        }
        tank.h = h;
        tank.w = w;
        tank.xc = xc;
        tank.yc = yc;

        let ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.moveTo(xc - w / 2, 0.2 * h);
        ctx.lineTo(xc - w / 2, 1.2 * h);
        ctx.lineTo(xc + w / 2, 1.2 * h);
        ctx.lineTo(xc + w / 2, 0.2 * h);
        ctx.stroke();
    }
}

function updateValue(value, tank) {
    if (value >= 0 && value <= 100) {
        let texto = value * tank.capacity / 100 + " lts";
        document.getElementById(tank.labelID).innerHTML = texto;

        let canvas = document.getElementById(tank.canvasID);
        if (canvas.getContext) {
            let ctx = canvas.getContext('2d');
            if (value > tank.min_level) {
                ctx.fillStyle = "#0000FF";
            } else {
                ctx.fillStyle = "#FF0000";
            }

            ctx.clearRect(tank.xc - tank.w / 2 + 1, 0.2 * tank.h + 1, tank.w - 2,
                tank.h - 2);
            ctx.fillRect(tank.xc - tank.w / 2 + 1,
                (1 - value / 100) * (tank.h - 2) + 0.2 * tank.h + 1,
                tank.w - 2, (value / 100) * (tank.h - 2));
        }
    } else {
        document.getElementById(tank.labelID).innerHTML = "Valor incorrecto!!";
    }
}

function configure_upperTank() {
    configuredTank = uTank;
    configureTank();
}

function configure_lowerTank() {
    configuredTank = lTank;
    configureTank();
}

function configureTank() {
    w3_close();
    document.getElementById("tank_config").style.display = "block";
    document.getElementById("config_tank_label").innerHTML = configuredTank.label;
    // Capacity
    document.getElementById("inputCapacity").value = configuredTank.capacity + "lts";
    // Height
    document.getElementById("inputHeight").value = configuredTank.height + "cm";
    // Gap
    document.getElementById("inputGap").value = configuredTank.gap + "cm";
    // Restart
    document.getElementById("slideRestart").value = configuredTank.restart;
    document.getElementById("labelRestart").innerHTML = "Reinicio: " + configuredTank.restart + "%";
    // Min
    document.getElementById("slideMin").value = configuredTank.min;
    document.getElementById("labelMin").innerHTML = "Mínimo: " + configuredTank.min + "%";
}

function setTankGap() {
    value = parseInt(document.getElementById("inputGap").value);
    if (value >= 10 && value <= 300) {
        document.getElementById("inputGap").value = value + "cm";
        configuredTank.gap = value;
    }
}

function setTankHeight() {
    value = parseInt(document.getElementById("inputHeight").value);
    if (value >= 20 && value <= 300) {
        document.getElementById("inputHeight").value = value + "cm";
        configuredTank.height = value;
    }
}

function setTankCapacity() {
    value = parseInt(document.getElementById("inputCapacity").value);
    if (value >= 0 && value <= 100000) {
        document.getElementById("inputCapacity").value = value + "lts";
        configuredTank.capacity = value;
    }
}

function setTankRestart() {
    configuredTank.restart = parseInt(document.getElementById("slideRestart").value);
    document.getElementById("labelRestart").innerHTML = "Reinicio: " + configuredTank.restart + "%";
}

function setTankMin() {
    configuredTank.min = parseInt(document.getElementById("slideMin").value);
    document.getElementById("labelMin").innerHTML = "Mínimo: " + configuredTank.min + "%";
}

function configure_pump() {
    w3_close();
    document.getElementById("pump_config").style.display = "block";
    document.getElementById("config_pump_label").innerHTML = "Bomba";

    // Maximum running time
    document.getElementById("inputTmax").value = myPump.Tmax + "min";
    // Start capacitor connceted dely
    document.getElementById("slideStart").value = myPump.startDelay;
    document.getElementById("labelStart").innerHTML = "Arranque: " + myPump.startDelay + "s";
}

function setPumpTmax() {
    value = parseInt(document.getElementById("inputTmax").value);
    if (value >= 0 && value <= 120) {
        document.getElementById("inputTmax").value = value + "min";
        myPump.Tmax = value;
    }
}

function setPumpStart() {
    myPump.startDelay = parseInt(document.getElementById("slideStart").value);
    document.getElementById("labelStart").innerHTML = "Arranque: " + myPump.startDelay + "s";
}

function startPump() {

}

function stopPump() {

}

function ackPump() {

}

function savePumpConfigs() {
    w3_close();
    //TODO actually save configurations
}

function saveTankConfigs() {
    w3_close();
    //TODO actually save configurations
}

function initPage() {
    loadTankData(uTank)
    loadTankData(lTank)
    loadPumpData(myPump)
    resetIndicators()
    updateValues()
}

function updateValues() {
    fetch("/get_values").then(function (response) {
        var contentType = response.headers.get("content-type");
        if (contentType && contentType.indexOf("application/json") !== -1) {
            return response.json().then(function (json) {
                updateValue(json.uLevel, uTank)
                updateValue(json.lLevel, lTank)
                myPump.state = json.pumpState
            });
        } else {
            console.log("Oops, we haven't got JSON!");
        }
    });

    setTimeout(updateValues, 2000)
}

function startWebrepl(state){
    fetch("/webrepl?state=" + state)
}

function loadPumpData(pump) {
    fetch("/get_indicator_config?indicator=" + pump.canvasID).then(function (response) {
        var contentType = response.headers.get("content-type");
        if (contentType && contentType.indexOf("application/json") !== -1) {
            return response.json().then(function (json) {
                pump.Tmax = json.Tmax
                pump.startDelay = json.startDelay
            });
        } else {
            console.log("Oops, we haven't got JSON!");
        }
    });
}

function loadTankData(tank) {
    fetch("/get_indicator_config?indicator=" + tank.canvasID).then(function (response) {
        var contentType = response.headers.get("content-type");
        if (contentType && contentType.indexOf("application/json") !== -1) {
            return response.json().then(function (json) {
                tank.min_level = json.min_level
                tank.capacity = json.capacity
                tank.min = json.min
                tank.restart = json.restart
                tank.height = json.height
                tank.gap = json.gap
            });
        } else {
            console.log("Oops, we haven't got JSON!");
        }
    });
}

function resetIndicators() {
    updateValue(0, uTank);
    updateValue(0, lTank);
    drawLowerTankTemplate(lTank);
    drawUpperTankTemplate(uTank);
    drawPumpTemplate(myPump);
    animatePump()
}
// Script to open and close sidebar
function w3_open() {
    document.getElementById("mySidebar").style.display = "block";
    document.getElementById("myOverlay").style.display = "block";
}

function w3_close() {
    document.getElementById("mySidebar").style.display = "none";
    document.getElementById("myOverlay").style.display = "none";
    document.getElementById("menu_upperTank").style.display = "none";
    document.getElementById("menu_lowerTank").style.display = "none";
    document.getElementById("menu_pump").style.display = "none";
    document.getElementById("tank_config").style.display = "none";
    document.getElementById("pump_config").style.display = "none";
}

// Modal Image Gallery
function onClick(element) {
    document.getElementById("img01").src = element.src;
    document.getElementById("modal01").style.display = "block";
    var captionText = document.getElementById("caption");
    captionText.innerHTML = element.alt;
}
// Menu Upper Tank
function menu_upperTank() {
    document.getElementById("menu_upperTank").style.display = "block";
}
// Menu Lower Tank
function menu_lowerTank() {
    document.getElementById("menu_lowerTank").style.display = "block";
}
// Menu Pump
function menu_pump() {
    document.getElementById("menu_pump").style.display = "block";
}
window.onresize = resetIndicators;