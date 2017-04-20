#include "udp.h"

struct udp_datagram
{
    udp_port_t port_src;
    udp_port_t port_dst;
    uint16_t total_len;
    uint16_t checksum;
    unsigned char payload[];
};

int udp_port_bin(udp_port_t port)
{
    return port[0] + (port[1] << 8);
}

void udp_bin_port(udp_port_t port, int bin_port)
{    
    port[0] = bin_port&0x00FF;
    port[1] = (bin_port&0xFF00) >> 8;
}

/*
 * int udp_open ()
 *
 * DESCRIPCI�N:
 *   Esta funci�n inicializa el protocolo udp para poder enviar datos.
 *
 * PAR�METROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si se ha podido inicializar el protocolo.
 *
 * ERRORES:
 *   La funci�n devuelve '-1' si se ha producido alg�n error.
 */
int udp_open()
{
    if (ipv4_open()==-1)
    {
        fprintf(stderr,"udp_open(): ERROR in ipv4_open()\n");
        exit(-1);
    }

    return 0;
}

/*
 * int udp_close ()
 *
 * DESCRIPCI�N:
 *   Esta funci�n finaliza el protocolo udp.
 *
 * PAR�METROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si se ha podido finalizar la instancia del protocolo.
 *
 * ERRORES:
 *   La funci�n devuelve '-1' si se ha producido alg�n error.
 */
int udp_close()
{
    if (ipv4_close() == -1)
    {
        fprintf(stderr,"udp_close(): ERROR in ipv4_close()\n");
        exit(-1);
    }

    return 0;
}

/*
 * int udp_send(udp_port_t port_src, udp_port_t port_dst, ipv4_addr_t ipv4_src,
 *                unsigned char *payload, int payload_len);
 *
 * DESCRIPCI�N:
 *   Esta funci�n permite enviar un datagrama udp especificando el protocolo
 *   de la capa superior.
 *
 * PAR�METROS:
 *     'port_src': Puerto origen.
 *   'port_dst': Puerto destino.
 *   'ipv4_dst': Direcci�n IP destino.
 *       'payload': Payload a enviar.
 * 'payload_len': Longitud en bytes del payload.
 *
 * VALOR DEVUELTO:
 *   Devuelve el n�mero de bytes enviados.
 *
 * ERRORES:
 *   La funci�n devuelve '-1' si se ha producido alg�n error.
 */
 int udp_send(udp_port_t port_src, udp_port_t port_dst, ipv4_addr_t ipv4_dst,
              unsigned char *payload, int payload_len)
{
    unsigned char udp_datagram[IPv4_MAX_PAYLOAD];

    udp_bin_port(((struct udp_datagram *)udp_datagram)->port_src, htons(udp_port_bin(port_src)));
    udp_bin_port(((struct udp_datagram *)udp_datagram)->port_dst, htons(udp_port_bin(port_dst)));
    
    
    ((struct udp_datagram *)udp_datagram)->total_len = htons(UDP_HEAD_SIZE + payload_len);
    //udp_datagram,checksum = udp_checksum();// PENDIENTE!!!!
    memcpy (((struct udp_datagram *)udp_datagram)->payload, payload, payload_len);

    memset(&(((struct udp_datagram *)udp_datagram)->checksum), 0 ,2);
    
    int bytes_sent;

    if ((bytes_sent = ipv4_send (ipv4_dst, UDP_PROTOCOL, udp_datagram,
                                 UDP_HEAD_SIZE+payload_len)) < 0)
    {
        fprintf(stderr, "udp_send(): ERROR en ipv4_send()\n");
        return -1;
    }

    return (bytes_sent - UDP_HEAD_SIZE);
}

/*
 * int udp_recv(udp_port_t port_src, ipv4_addr_t ipv4_src, udp_port_t port_dst,
 *                 unsigned char *buffer, long int timeout );
 *
 * DESCRIPCI�N:
 *   Esta funci�n permite recibir un datagrama udp desde el nivel de enlace
 *     especificando el protocolo de la capa superior.
 *
 * PAR�METROS:
 * 'port_src': Puntero donde se almacenara el puerto origen.
 * 'ipv4_src': Variable donde se almacenar� la IP origen.
 * 'port_dst': Puerto destino.
 *     'buffer': Buffer para almacenar los datos recibidos.
 *  'timeout': Tiempo en milisegundos que debe esperarse a recibir un datagrama
 *             antes de retornar. Un n�mero negativo indicar� que debe
 *             esperarse indefinidamente, mientras que con un '0' la funci�n
 *             retornar� inmediatamente, se haya recibido o no un datagrama.
 *
 * VALOR DEVUELTO:
 *   Devuelve el n�mero de bytes recibidos.
 *
 * ERRORES:
 *   La funci�n devuelve '-1' si se ha producido alg�n error.
 */
int udp_recv(udp_port_t port_src, ipv4_addr_t ipv4_src, udp_port_t port_dst,
             unsigned char *buffer, long int timeout )
{
    timerms_t timer;
    timerms_reset(&timer, timeout);
    
    int bytes_read, recv = 0;
    unsigned char udp_buffer[IPv4_MAX_PAYLOAD];
    struct udp_datagram *udp_dgm;
    
    do
    {
        long int time_left = timerms_left(&timer);
        
        if ((bytes_read = ipv4_recv (ipv4_src, UDP_PROTOCOL, udp_buffer, time_left)) < 0)
        {
            fprintf(stderr, "udp_recv(): ERROR en ipv4_recv()\n");
            return -1;
        }
        else if (bytes_read == 0)
        {
            return 0;
        }
        udp_dgm = (struct udp_datagram *)udp_buffer;
        
        //Mensajes de depuracion
        printf("DEBUG: Paquete UDP:\n");
        char ip_src[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(ipv4_src,ip_src);
        printf("Origen: IP:%s,Puerto:%d\n",ip_src, ntohs(udp_port_bin(udp_dgm->port_src)));
        printf("Destino: IP:La ip del equipo...,Puerto:%d\n",ntohs(udp_port_bin(udp_dgm->port_dst)));
        printf("Puerto esperado: %d\n", udp_port_bin(port_dst));
        //Mensajes de depuracion
        
        uint16_t p_src_bin = ntohs(udp_port_bin(udp_dgm->port_src));
        uint16_t p_dst_bin = ntohs(udp_port_bin(udp_dgm->port_dst));
		
		udp_port_t p_src;
		udp_bin_port(p_src, p_src_bin);
        
		udp_port_t p_dst;
		udp_bin_port(p_dst, p_dst_bin);
		
        if (memcmp(&p_dst, port_dst, sizeof(udp_port_t)) == 0)
        {            
            memcpy(port_src, &p_src, UDP_PORT_SIZE);
            recv = 1;
            puts("Puerto destion y puerto esperado coinciden.");
        }   
    }
    while(!recv);
    
    uint32_t payload_len = ntohs(udp_dgm->total_len) - UDP_HEAD_SIZE;
    
    memcpy(buffer, udp_dgm->payload, payload_len);
    memcpy(port_src, udp_dgm->port_src, UDP_PORT_SIZE);
    
    printf("Origen: Puerto:%d\n",ntohs(udp_port_bin(port_src)));
    printf("Destino: Puerto:%d\n",ntohs(udp_port_bin(udp_dgm->port_dst)));
    printf("Tamaño payload | paquete:%d | %d\n", payload_len, bytes_read);

    return payload_len;
}
