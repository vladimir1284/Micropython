<script lang="ts">
    import { onMount } from 'svelte';

    export let id: string; 
    export let state: boolean;

    $: {
        try{
            updateLightValue(state);
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
        let canvas = document.getElementById("light"+id) as HTMLCanvasElement;
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

            ctx.beginPath()
            ctx.translate(xc, yc - 5 * pxl)
            ctx.arc(0, 0, 12 * pxl, 3 * Math.PI / 4, Math.PI / 4)
            ctx.lineTo(6 * pxl, 15.5 * pxl)
            ctx.lineTo(-6 * pxl, 15.5 * pxl)
            ctx.closePath()
            ctx.stroke()
            ctx.beginPath()
            ctx.fillRect(-5 * pxl, 17 * pxl, 10 * pxl, 2 * pxl)
            ctx.fillRect(-5 * pxl, 20 * pxl, 10 * pxl, 2 * pxl)
            ctx.beginPath()
            ctx.arc(0, 18.4 * pxl, 7 * pxl, Math.PI / 4, 3 * Math.PI / 4)
            ctx.fill()
            ctx.translate(-xc, -yc + 5 * pxl)
        }
    }


    function updateLightValue(value: boolean) {
        if (ctx != undefined){
            if (value) {
                display = "Encendida";
                ctx.fillStyle = "#fafd08";
                ctx.beginPath();
                ctx.translate(xc, yc - 5 * pxl);
                ctx.arc(0, 0, 12 * pxl, 3 * Math.PI / 4, Math.PI / 4);
                ctx.lineTo(6 * pxl, 15.5 * pxl);
                ctx.lineTo(-6 * pxl, 15.5 * pxl);
                ctx.closePath();
                ctx.fill();
                for (let i = 0; i < 5; i++) {
                    ctx.fillRect(14 * pxl, -pxl, 4 * pxl, 2 * pxl);
                    ctx.rotate(- Math.PI / 4);
                }
                ctx.fillStyle = "#000000";
                ctx.rotate(5 * Math.PI / 4);
                ctx.translate(-xc, -yc + 5 * pxl);
            } else {
                display = "Apagada";
                ctx.clearRect(0, 0, 2 * xc, 2 * yc);
                init();
            }
        }
    }

    // Do the job once the DOM was generated
    onMount(init);

</script>
<div class="container">
    <canvas id={"light"+id}></canvas>
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