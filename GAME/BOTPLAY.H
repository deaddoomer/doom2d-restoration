struct BotPlayer {
  BotPlayer();
  virtual ~BotPlayer();
  virtual int getkeys(player_t *p);
};

BotPlayer *BOT_new();
