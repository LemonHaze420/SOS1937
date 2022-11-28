// $Header$

// $Log$
// Revision 1.1  2000-02-29 08:44:42+00  jjs
// Added Header and Log lines
//

/* The Buttons used in the front end

	Ian Gledhill 12/01/2000
	Broadsword Interactive Ltd. */

class Button
{
public:
	Button(char *ButtonName, float Scale, int x, int y, float z);
	~Button();
	
	void Draw();
	void Highlight();
	void Lowlight();
	void Select();
	void Unselect();
	void Hide();


private:
	// The original name
	char *OriginalName;
	
	// The models and objects
	model *UActualModel, *HActualModel, *SActualModel;
	object *UActualObject, *UParentObject, *HActualObject, *HParentObject, *SActualObject, *SParentObject;

	// The X, Y and Z.
	int StartX, StartY;
	float StartZ;
};

struct ButtonNode
{
	Button *Node;
	ButtonNode *NextNode;
};

class ButtonBank
{
public:
	ButtonBank();
	~ButtonBank();

	void Add(Button *NewButton);
	void Remove(Button *OldButton);

	// Highlight a button
	void Highlight(int Index);
	void Lowlight(int Index);
	void Select(int Index);
	void Unselect(int Index);

	// Draw all the buttons.
	void DrawAll();
private:
	ButtonNode *ButtonList;
};