# CONTROL DE POSICIÓN DE UNA PELOTA EN LEVITACIÓN

Recientemente, me encontré por casualidad con un vídeo que mostraba cómo un secador de pelo puede hacer levitar una pelota de ping-pong, ilustrando el principio de Bernoulli. Esto despertó mi curiosidad sobre la posibilidad de ajustar la potencia del secador para controlar la altura a la que se mantiene la pelota. Inspirado por esta idea, esa misma noche comencé a experimentar con algunos componentes que tenía guardados y, poco a poco, di forma a la maqueta que les presento hoy. Es un proyecto bastante sencillo de realizar, aunque presenta algunos desafíos a la hora de identificar el sistema. La maqueta consiste en una pelota de ping-pong situada dentro de un tubo, la cual se eleva venciendo la fuerza de la gravedad gracias al aire suministrado por un ventilador.<br>
Las piezas amarillas están impresas en 3D y sirven para asegurar el caudal de aire y conectar el tubo. En la parte superior, el sensor HC-SR04 encaja justo con las dimensiones del diámetro interior del tubo. Si optáis por usar el sensor US-016 deberéis pegarlo con celo o diseñar una pieza para anclarlo.
<p align="center">
  <img src="https://garikoitz.info/blog/wp-content/uploads/2024/03/Maquetal_pelota_vent.png" width="450" height="460" alt="">
  <img src="https://garikoitz.info/blog/wp-content/uploads/2024/03/PXL_20240304_175235843.MP_-scaled.jpg" width="450" height="460" alt="">
</p>

<h2>Esquema de conexiones</h2>
<p align="center">
  <img src="https://garikoitz.info/blog/wp-content/uploads/2024/03/Esquema_Pelota_levita_bb.png" width="450" alt="conexiones">
</p>

<h2>Esquema de control</h2>
<p align="center">
  <img src="https://garikoitz.info/blog/wp-content/uploads/2024/03/Esquema_Control_pelota_vent.png" width="240" alt="">
</p>

<h2>Implementación</h2>
<p>Nuestro propósito es posicionar con precisión la pelota en un lugar específico dentro del tubo. Utilizamos un sensor de distancia ultrasónico para medir la distancia exacta de la pelota, expresada en centímetros. A continuación, ajustamos la velocidad de giro del ventilador empleando una señal de modulación de ancho de pulso (PWM, por sus siglas en inglés). Este método nos permite variar finamente la potencia aplicada al ventilador y, por ende, controlar la altura a la que se mantiene la pelota.</p>
<br>
https://www.youtube.com/watch?v=kyfIPwP09Ao
<br>
Tenéis más información en la entrada del blog:<br>
https://garikoitz.info/blog/2024/03/sintonizar-pid-con-arduino-control-de-posicion-de-una-pelota-en-levitacion/
