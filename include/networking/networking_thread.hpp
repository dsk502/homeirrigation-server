//void server_func(int is_added, int listeningPort);

class HomeIrrigationServer;

class NetworkingThread {
public:
    static int networking_thread_main(HomeIrrigationServer* home_irri_serv);
};