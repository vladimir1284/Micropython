<script lang="ts">
    import { onMount } from 'svelte';

    export let id: string; 
    export let state: boolean;

    $: {
        try{
            updateMovementValue(state);
        }
        finally{}
    }


    let display = "Apagada";

    let ctx: CanvasRenderingContext2D;
    let yc: number;
    let xc: number;
    let h: number;
    let w: number;
    let pxl: number;

    function init() {
        let canvas = document.getElementById("pir"+id) as HTMLCanvasElement;
        if (canvas.getContext) {

            w = 0.8 *canvas.width;
            h = w;

            if (h > (0.8 * canvas.height)) {
                h = 0.8 * canvas.height;
                w = h;
            }

            xc = 0.5 * canvas.width;
            yc = 0.5 * canvas.height;
            
            pxl = w / 48;

            ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

            ctx.translate(2 * w / 3, 0);

            // Movement Sensor
            ctx.beginPath();
            ctx.moveTo(30 * pxl, 50 * pxl);
            ctx.lineTo(30 * pxl, 42 * pxl);
            ctx.lineTo(37 * pxl, 37 * pxl);
            ctx.lineTo(34 * pxl, 26 * pxl);
            ctx.lineTo(41 * pxl, 26 * pxl);
            ctx.lineTo(45 * pxl, 27 * pxl);
            ctx.lineTo(49 * pxl, 31 * pxl);
            ctx.lineTo(48 * pxl, 33 * pxl);
            ctx.lineTo(44 * pxl, 29 * pxl);
            ctx.lineTo(41 * pxl, 28 * pxl);
            ctx.lineTo(42 * pxl, 38 * pxl);
            ctx.lineTo(34 * pxl, 43 * pxl);
            ctx.lineTo(32 * pxl, 49 * pxl);
            ctx.closePath();
            ctx.fill();

            ctx.beginPath();
            ctx.arc(35 * pxl, 21 * pxl, 3 * pxl, 0, 2 * Math.PI);
            ctx.fill();

            ctx.fillRect(29 * pxl, 31 * pxl, 6 * pxl, 2 * pxl);

            ctx.beginPath();
            ctx.moveTo(41 * pxl, 40 * pxl);
            ctx.lineTo(43 * pxl, 39 * pxl);
            ctx.lineTo(49 * pxl, 48 * pxl);
            ctx.lineTo(49 * pxl, 49 * pxl);
            ctx.lineTo(47 * pxl, 49 * pxl);
            ctx.closePath();
            ctx.fill();

            ctx.beginPath();
            ctx.arc(18 * pxl, 11 * pxl, 3 * pxl, 0, Math.PI);
            ctx.fill();

            ctx.lineWidth = 3;
            for (let i = 0; i < 4; i++) {
                ctx.beginPath();
                ctx.arc(18 * pxl, 11 * pxl, (6 + 4 * i) * pxl, 
                    Math.PI / 12, 7 * Math.PI / 12);
                ctx.stroke();
            }
            ctx.translate(-2 * w / 3, 0);
        }
    }


    function updateMovementValue(value: boolean) {
        if (ctx != undefined){
            if (value) {
                display = "Activado";
                ctx.translate(2 * w / 3, 0)
                ctx.strokeStyle = "#fafd08";
                ctx.strokeRect(10 * pxl, 5 * pxl, 45 * pxl, 50 * pxl);
                ctx.strokeStyle = "#000000";
                ctx.translate(-2 * w / 3, 0)
            }
            else {
                display = "Inactivo";
                ctx.translate(2 * w / 3, 0)
                ctx.clearRect(9 * pxl, 4 * pxl, 47 * pxl, 52 * pxl);
                ctx.translate(-2 * w / 3, 0)
                init();
            }
        }
    }

    // Do the job once the DOM was generated
    onMount(init);

</script>
<div class="container">
    <canvas id={"pir"+id}></canvas>
    <div class="status"> {display} </div>
</div>

<style>
    .container{
        display: grid;
        grid-template-columns: auto;
    }
	.status {
		text-align: center;
    }
</style>