# C dilinde TCP socket programlama özet notlar (Windows - Unix)

![tcp_handshake_local](https://user-images.githubusercontent.com/101992799/236589591-dc1c3858-b1ec-496a-82d4-8c871529d6f1.png)

### #0 - Fonksiyonlarda hata yönetimi
Tüm fonksiyonlarda eğer bir hata oluşursa işlevler -1 değerini döndürür.
Bu durumda errno isimli global değişken hata kodunu tutar (bkz. perror() man sayfası.)

## FUNCTION list

    1.  socket(int domain, int type, int protocol);                    - Al Şu Dosya Tanımlayıcıyı!
    2.  bind(int sockfd, struct sockaddr* my_addr, int addrlen);       - Hangi Port Üzerindeyim?
    3.  connect(int sockfd, struct sockaddr* serv_addr, int addrlen);  - Uzaktaki bir bilgisayara nasıl bağlanırım?
    4.  listen(int sockfd, int backlog);                               - Biri Beni Arayabilir mi Acaba?
    5.  accept(int sockfd, void* addr, int* addrlen);                  - "3490" Numaralı Portu Aradığınız İçin Teşekkürler
    6.  send(int sockfd, const void* msg, int len, int flags);         - Veri Gönder
    7.  recv(int sockfd, void* buf, int len, unsigned int flags);      - Veri Al
    8.  close(int sockfd); _LINUX || closesocket(int sockfd); WINDOWS  - İletişimi Kapat
    9.  getpeername(int sockfd, struct sockaddr* addr, int* addrlen);  - Soketinin diğer tarafındaki kim?
    10. gethostname(char* hostname, size_t size);                      - Programım hangi makine adresi üzerinde çalışıyor?

### #1 - socket(int domain, int type, int protocol);

    int socket(int domain, int type, int protocol);

        PARAMETERS:

        int domain          =>  AF_INET
        int type            =>  SOCK_STREAM, SOCK_DGRAM
        int protocol        =>  0


### #2 - bind(int sockfd, struct sockaddr *my_addr, int addrlen);

    int bind(int sockfd, struct sockaddr *my_addr, int addrlen);

        PARAMETERS:

        int sockfd          =>  Socket File Descriptor

        struct sockaddr {
            unsigned short  sa_family;      // adres ailesi, AF_XXX
            char            sa_data[14];    // protokol adresinin 14 byte'ı
        };

            struct sockaddr_in {
                short int          sin_family;  // Adres ailesi
                unsigned short int sin_port;    // Port numarası
                struct in_addr     sin_addr;    // Internet adresi
                unsigned char      sin_zero[8]; // struct sockaddr ile aynı boyda
            };

                // Internet adresi (tarihi sebeplerden ötürü korunmakta)
                struct in_addr {
                    unsigned long s_addr; // 32-bit yani 4 bytes
                };

            >  sin_port ve sin_addr değikenlerinin de Ağ Byte Sırasında bulunmaları gerek. (BIG-ENDIAN _> "Kıymetlisi Başta Bayt Sırası")

        >  struct sockaddr -> adresinizi yani port numaranızı ve IP adresinizi barındırır.

        int addrlen         =>  sizeof(struct sockaddr)


        
    # bind() ile ilgili ek bilgiler.

    my_addr.sin_port = 0;                   // kullanılmayan herhangi bir port'u seçer
    my_addr.sin_addr.s_addr = INADDR_ANY;   // IP adresimi kullan 


### #3 - listen(int sockfd, int backlog)

    int listen(int sockfd, int backlog);

        PARAMETERS:

        int sockfd      =>  Socket File Descriptor to listen.
        int backlog     =>  gelen çağrı kuyruğunda izin verilen bağlantı sayısını gösterir.


### #4 - accept(int sockfd, void* addr, int* addrlen);

    int accept(int sockfd, void* addr, int* addrlen);

        PARAMETERS:

        int sockfd      =>  sockfd listen() ile dinlediğiniz soket tanımlayıcıdır.
        void* addr      =>  
            addr yerel olarak kullandığınız struct sockaddr_in yapısını gösteren bir göstergedir. Gelen bağlantılarla ilgili bilgiler burada barındırılacak (yani bu yapıyı kullanarak gelen bağlantının hangi bilgisayar ve hangi porttan geldiğini öğrenebilirsiniz).

        int* addrlen    =>  
            addrlen yerel bir tamsayı değişkendir ve kullanılmadan önce accept() sizeof (struct sockaddr_in) değerini almalıdır. accept() işlevi bu değerden daha fazla sayıda baytı addr içine yerleştirmeyecektir. Eğer söz konusu değerden daha azını yerleştirirse o zaman da addrlen değerini, bunu yansıtacak şekilde değiştirecektir.


### #5 - send() ve recv() - (UDP için sendto() ve recvfrom())

    int send(int sockfd, const void* msg, int len, int flags);

        PARAMETERS:

        int sockfd              =>  üzerinden veri göndereceğiniz sokettir.
                                    (size socket() veya accept() tarafından sağlanmış olabilir)

        const void* msg         =>  göndermek istediğiniz mesajı gösteren bir göstergedir.

        int len                 =>  len değişkeni bu verinin(msg) byte cinsinden uzunluğudur.

        int flags               =>  flags parametresini 0 olarak bırakabilirsiniz.
                                    (Bu parametre ile ilgili ayrıntılı bilgi için bkz. send() man sayfası.)


    >  send() değer olarak gönderilen bayt miktarını döndürür.
    >  bu sizin gönderilmesini istediğiniz miktardan az olabilir! Gördüğünüz gibi siz ona bir yığın veri göndermesini söylersiniz ancak o bazen bunun tamamı ile başa çıkamayabilir. Elinden geldiği kadarını gönderir ve geriye kalan veriyi yeniden göndermek sizin sorumluluğunuzdadır. 
    >  Unutmayın, eğer send() işlevinin döndürdüğü değer len değişkenindeki değer kadar değilse göndermek istediğiniz verinin geriye kalanını göndermek sizin işinizdir. 
    >  Eğer paket küçükse (1k civarı) bu işlev büyük ihtimalle tüm veriyi bir seferde gönderebilecektir.


    int recv(int sockfd, void* buf, int len, unsigned int flags);

        PARAMETERS:

        int sockfd              =>  sockfd üzerinden okuma işlemini gerçekleştireceğiniz sokettir.

        void* buf               =>  buf okunan verinin yazılacağı bellek bölgesinin başlangıç adresini gösteren göstergedir.

        int len                 =>  len ise verinin yazılacağı tamponun (buffer) azami boyudur.

        unsigned int flags      =>  flags yine 0 değerini alabilir.
                                    (Ayrıntılı bilgi için recv() man sayfasına bakınız.)


    >  recv() okunduktan sonra tampona yazılan bayt miktarını döndürür.
    >  recv() 0 değerini döndürebilir! Bunun tek bir anlamı vardır: karşı taraf bağlantıyı kesmiş! Döndürülen değerin 0 olması recv() işlevinin size "karşı taraf bağlantıyı kesti" mesajını vermesi demektir.


### #6 - connect() - uzaktaki bir bilgisayara nasıl bağlanırız 

    #define DEST_IP     "10.12.110.57"
    #define DEST_PORT   23 

    int connect(int sockfd, struct sockaddr* serv_addr, int addrlen);

        PARAMETERS:

        int sockfd                      =>  sockfd dediğimiz değişken socket() sistem çağrısı tarafından döndürülmüş olan soket dosya tanımlayıcısının değerini tutar.
        struct sockaddr* serv_addr      =>  serv_addr ise struct sockaddr türünde bir değişkendir ve hedef port ile IP adres bilgilerini barındırır. 
        int addrlen                     =>  addrlen değişkeni de sizeof(struct sockaddr) değerini alırsa iyi olur.


    >  Diyelim ki bir telnet programısınız. Sizi kullanan kişi size bir soket dosya tanımlayıcısı edinmeniz için komut veriyor. 
    >  Siz de bu komuta uyup socket() işlevini çağırıyorsunuz.
    >  Sonra, kullanıcı sizden "10.12.110.57" adresine ve "23" numaralı porta bağlanmanızı istiyor (standart telnet portu). Aha! Şimdi ne yapacaksınız?


    int sockfd;
    struct sockaddr_in dest_addr;             // hedef adres

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // hata denetimi yapın!

    dest_addr.sin_family = AF_INET;          // konak bayt sıralaması
    dest_addr.sin_port = htons(DEST_PORT);   // short, ağ bayt sıralaması
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    memset(&(dest_addr.sin_zero), '\0', 8);  // yapının geriye kalanını sıfırla

    // connect() işlevini çağırdıktan sonra hata denetimi yap!
    connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));


### #7 - close()

    close(sockfd);          // on LINUX
    closesocket(sockfd);    // on WINDOWS

    >  Böylece artık bu sokete ne yazılabilir ne de buradan veri okunabilir. 
    >  Diğer uçta bunları yapmaya çalışan kişi artık bir hata mesajı ile karşılaşacaktır.


### #8 - shutdown()

    int shutdown(int sockfd, int how);

        PARAMETERS:

        int sockfd      =>  sockfd kapatmak istediğiniz soket dosya tanımlayıcısıdır.
        int how         =>  how değişkeni de şu değerlerden birini alabilir:

                            0 -- Bundan sonraki okumalara izin verme.
                            1 -- Bundan sonraki göndermelere (yazmalara) izin verme.
                            2 -- Bundan sonraki göndermelere ve okumalara izin verme (close() işlevinin yaptığı gibi)


    >  shutdown() başarılı olarak görevini tamamlarsa 0 döndürür ama eğer bir hata ile karşılaşırsa -1 döndürür.
    >  Eğer shutdown() işlevini bağlantısız veri paketi soketleri üzerinde kullanırsanız bu soketler artık send() ve recv() işlevleri tarafından kullanılamaz hale gelirler.
         (bunları, connect() ile bağlanmak istediğinizde kullanabileceğinizi unutmayın)
        
    >  Bir başka önemli nokta da: shutdown() aslında dosya tanımlayıcısını kapatmaz sadece kullanılabilirliğini değiştirir. Soket tanımlayıcısını gerçekten iptal etmek istiyorsanız close() kullanmalısınız.


### #9 - getpeername() -- Kimsiniz?

    >  getpeername() bağlantılı veri akış soketinin diğer tarafında kim olduğunu söyler.

    int getpeername(int sockfd, struct sockaddr* addr, int* addrlen)

        PARAMETERS:

        int sockfd                  =>  sockfd bağlantılı veri akış soketinin tanımlayıcısıdır.
        struct sockaddr* addr       =>  addr, struct sockaddr (veya struct sockaddr_in) türündeki bir göstergedir ki bu da iletişimin diğer ucundaki konak ile ilgili bilgileri tutar.
        int* addrlen                =>  addrlen, int türündeki göstergedir, alması gereken ilk değer ise sizeof(struct sockaddr) olarak verilir.


    >  Bir kere karşı tarafın adres bilgisine eriştikten sonra inet_ntoa() veya gethostbyaddr() işlevleri ile daha fazla bilgi edinebilirsiniz.


### #10 - gethostname() -- Ben kimim?

    >  Programınızın üzerinde çalıştığı konağın ismini döndürür.
    >  Bu isim daha sonra gethostname() tarafından makinanızın IP adresini tespit etmek için kullanılabilir.

    int gethostname(char* hostname, size_t size);

        PARAMETERS:

        char* hostname      =>  hostname işlev çağrıldıktan sonra bilgisayarın ismini barındıracak karakter dizisinin göstergesidir.
        size_t size         =>  size değişkeni de hostname dizisinin bayt cinsinden uzunluğudur.


### # utility functions
*   htons() -- "Host to Network Short" -- konaktan ağa short  (little-endian to big-endian)
*   htonl() -- "Host to Network Long" -- konaktan ağa long
*   ntohs() -- "Network to Host Short" -- ağdan konağa short
*   ntohl() -- "Network to Host Long" -- ağdan konağa long

*   inet_ntoa(struct in_addr in) -- "Network to ASCII"
