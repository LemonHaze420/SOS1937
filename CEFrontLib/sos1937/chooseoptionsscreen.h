/* The options screen */

class ChooseOptionsScreen : public Screen
{
public:
	ChooseOptionsScreen(Alphabet *);
	virtual ~ChooseOptionsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:

};