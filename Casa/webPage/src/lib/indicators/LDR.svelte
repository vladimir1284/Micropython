<script lang="ts">
	import { onMount } from 'svelte';

	export let id: string;
	export let luminosity = 0;

	$: {
		try {
			updateLuminosityValue(luminosity);
		} finally {
		}
	}

	let display = 'Apagada';

	let ctx: CanvasRenderingContext2D;
	let yc: number;
	let xc: number;
	let h: number;
	let w: number;
	let pxl: number;

	function init() {
		let canvas = document.getElementById('ldr' + id) as HTMLCanvasElement;
		if (canvas.getContext) {
			w = 0.8 * canvas.width;
			h = w;

			if (h > 0.8 * canvas.height) {
				h = 0.8 * canvas.height;
				w = h;
			}

			xc = 0.5 * canvas.width;
			yc = 0.5 * canvas.height;

			pxl = w / 48;

			ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

			ctx.translate(xc, 20 * pxl);
			ctx.beginPath();
			ctx.lineWidth = 4;
			ctx.arc(0, 0, 6 * pxl, 0, 2 * Math.PI);
			ctx.stroke();

			ctx.beginPath();
			//ctx.translate(xc, 20 * pxl);
			for (let i = 0; i < 8; i++) {
				ctx.fillRect(8 * pxl, -pxl, 4 * pxl, 2 * pxl);
				ctx.rotate(Math.PI / 4);
			}
			ctx.translate(-xc, -20 * pxl);
		}
	}

	function updateLuminosityValue(value: number) {
		if (ctx != undefined) {
			if (value < 0) {
				value = 0;
			}
			if (value > 100) {
				value = 100;
			}
			display = value + '%';
			value = Math.round(value / 10);
			ctx.beginPath();
			ctx.translate(41 * pxl, 45 * pxl);
			// ctx.translate(41 * pxl, 45 * pxl);
			ctx.clearRect(-pxl, -pxl, 45 * pxl, 5 * pxl);
			for (let i = 0; i < 10; i++) {
				if (value > i) {
					ctx.fillStyle = '#000000';
				} else {
					ctx.fillStyle = '#ababab';
				}
				ctx.fillRect(i * 4 * pxl, 0, 2 * pxl, 3 * pxl);
			}
			ctx.fillStyle = '#000000';
			ctx.translate(-41 * pxl, -45 * pxl);
		}
	}

	// Do the job once the DOM was generated
	onMount(init);
</script>

<div class="container">
	<canvas id={'ldr' + id} />
	<div class="status">{display}</div>
</div>

<style>
	.container {
		display: grid;
		grid-template-columns: auto;
	}
	.status {
		text-align: center;
	}
</style>
