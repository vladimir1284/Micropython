<script lang="ts">
    import { onMount } from 'svelte';

	const STATES: Record<string, string> = {
		OPEN: 'Abierta',
		CLOSE: 'Cerrada',
		ERROR: 'Error'
	};

    export let id: string; 
    export let state: string;

    $: {
        try{
            updateDoorValue(state);
        }
        finally{};
    }


    let display = "Abierta";

    let ctx: CanvasRenderingContext2D;
    let yc: number;
    let xc: number;
    let h: number;
    let w: number;
    let pxl: number;

    function init() {
        let canvas = document.getElementById("door"+id) as HTMLCanvasElement;
        if (canvas.getContext) {

            w = 0.8 *canvas.width;
            h = w;

            if (h > (0.8 * canvas.height)) {
                h = 0.8 * canvas.height;
                w = h;
            }

            xc = 0.5 * canvas.width;
            yc = 0.5 * canvas.height;
            
            pxl = w / 100;

            ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

            ctx.lineWidth = 2;
            ctx.translate(xc / 2, yc / 4);
            if (state == "OPEN") {
                ctx.beginPath();
                ctx.moveTo(78 * pxl, 86 * pxl);
                ctx.lineTo(87 * pxl, 86 * pxl);
                ctx.lineTo(87 * pxl, 11 * pxl);
                ctx.lineTo(64 * pxl, 11 * pxl);
                ctx.lineTo(78 * pxl, 18 * pxl);
                ctx.lineTo(78 * pxl, 86 * pxl);
                ctx.lineTo(49 * pxl, 95 * pxl);
                ctx.lineTo(49 * pxl, 5 * pxl);
                ctx.lineTo(64 * pxl, 11 * pxl);
                ctx.moveTo(49 * pxl, 18 * pxl);
                ctx.lineTo(42 * pxl, 18 * pxl);
                ctx.lineTo(42 * pxl, 86 * pxl);
                ctx.lineTo(34 * pxl, 86 * pxl);
                ctx.lineTo(34 * pxl, 40 * pxl);
                ctx.moveTo(34 * pxl, 26 * pxl);
                ctx.lineTo(34 * pxl, 11 * pxl);
                ctx.lineTo(49 * pxl, 11 * pxl);

                ctx.moveTo(56 * pxl, 47 * pxl);
                ctx.lineTo(56 * pxl, 61 * pxl);
            } else {
                ctx.strokeRect(34 * pxl, 11 * pxl, 53 * pxl, 75 * pxl);
                ctx.strokeRect(42 * pxl, 18 * pxl, 36 * pxl, 68 * pxl);
                ctx.clearRect(30 * pxl, 25 * pxl, 5 * pxl, 13 * pxl);
                ctx.moveTo(49 * pxl, 47 * pxl);
                ctx.lineTo(49 * pxl, 57 * pxl);
            }
            ctx.moveTo(37 * pxl, 29 * pxl);
            ctx.lineTo(37 * pxl, 36 * pxl);
            ctx.moveTo(31 * pxl, 29 * pxl);
            ctx.lineTo(31 * pxl, 36 * pxl);
            ctx.stroke();
            ctx.beginPath();
            ctx.arc(34 * pxl, 29 * pxl, 3 * pxl, Math.PI, 2 * Math.PI);
            ctx.arc(34 * pxl, 36 * pxl, 3 * pxl, 0, Math.PI);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(25 * pxl, 49 * pxl);
            ctx.lineTo(18 * pxl, 56 * pxl);
            ctx.moveTo(23 * pxl, 33 * pxl);
            ctx.lineTo(13 * pxl, 33 * pxl);
            ctx.moveTo(25 * pxl, 17 * pxl);
            ctx.lineTo(19 * pxl, 10 * pxl);
            ctx.stroke();

            ctx.translate(-xc / 2, -yc / 4);
        }
    }


    function updateDoorValue(value: string) {
        if (ctx != undefined){
            display = STATES[value];
            ctx.clearRect(xc / 3, yc / 5, 150 * pxl, 110 * pxl)
            init();
            if (value == "ERROR") {
                ctx.strokeStyle = "#ff0000";
                ctx.strokeRect(xc / 2, yc / 4, xc, 1.6 * yc);
                ctx.strokeStyle = "#000000";                
            }            
        }
    }

    // Do the job once the DOM was generated
    onMount(init);

</script>
<div class="container">
    <canvas id={"door"+id}></canvas>
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