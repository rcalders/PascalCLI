// Compiler.cpp: implementation of the CCompiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Compiler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompiler::CCompiler(CHPPASCALCompilerCtrl *dialogue)
{
	dlg = dialogue;

	// Initialise les variables
	VariablesGlobales.RemoveAll();
	VariablesLocales.RemoveAll();
	VariablesStatiques.RemoveAll();

	localStatique = NULL;
	localCste = NULL;

	// Initialise les param�tres pass�s aux fonctions
	VariablesParametres.RemoveAll();

	// Initialise les constantes non typ�es
	Constantes.RemoveAll();

	// Initialise les proc�dures et fonctions
	ProceduresEtFonctions.RemoveAll();

	// Initialise les USES
	ListeUses.RemoveAll();

	// Initialise les contr�le de FLUX pour instructions break, continue
	
	LabelBreak ="";
	LabelContinue = "";

	// Initialise le contexte proc�dural

	ProcName = "";
	ProcVersion = 0;
	
	// Chargement du fichier
	dlg->EtapeFichier (dlg->fileSource()); 
	FichierEntree = new CSyntaxe (dlg->directorySource(), dlg->fileSource(), dlg);
	IndexSource = FichierEntree->lexemes.GetHeadPosition();

	Erreur = FichierEntree->Erreur;

	generatedFile = new CLdjTextFile (dlg->directoryDestination(), "Resultat2.txt");
	EnableWriting = true;
}

void CCompiler::DoCompile()
{
	statusCompileSyntaxe = "Compilation";

	if (Erreur.Message=="")
	{
		// Construit les lexemes
		try
		{
			Compile ();
		}
		catch(char* err)
		{
			Erreur.Message = err;
		}
		catch(CString err)
		{
			Erreur.Message = err;
		}
		
		if (!VariablesGlobales.IsEmpty())
			GlobalMemory = VariablesGlobales.GetTail().Adresse+VariablesGlobales.GetTail().Type.Taille();
		else
			GlobalMemory = 0;
	}
}

void CCompiler::DoCheckSyntaxe()
{
	statusCompileSyntaxe = "V�rification de la syntaxe";

	if (Erreur.Message=="")
	{
		// Construit les lexemes
		try
		{
			CompileUnit ();
		}
		catch(char* err)
		{
			Erreur.Message = err;
		}
		catch(CString err)
		{
			Erreur.Message = err;
		}
		
		if (!VariablesGlobales.IsEmpty())
			GlobalMemory = VariablesGlobales.GetTail().Adresse+VariablesGlobales.GetTail().Type.Taille();
		else
			GlobalMemory = 0;
	}
}

CCompiler::~CCompiler()
{

}


//////////////////////////////////////////////////////////////////////
// Interface
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Fonctions auxiliaires d'entr�e - sortie
//////////////////////////////////////////////////////////////////////

void CCompiler::LireLexeme ()
{
	// Lit le lex�me
	if (IndexSource != NULL)
		lexeme = FichierEntree->lexemes.GetNext (IndexSource);
	else
		lexeme.Type=END_OF_FILE;
	
	// Met � jour l'emplacement actuel dans le fichier source
	Erreur.Ligne = lexeme.Ligne;
	Erreur.Abscisse = lexeme.Abscisse;

	// Remplace le lex�me par sa valeur si c'est une cste non typ�e
	if (lexeme.Type==ID)
	{
		POSITION position = Constantes.GetHeadPosition();
		if (position==NULL) return;
		while (true)
		{
			TCste cste = Constantes.GetNext(position);
			if (cste.nom==lexeme.Valeur)
			{
				lexeme.Type=cste.valeur.Type;
				lexeme.Valeur=cste.valeur.Valeur;
				return;
			}
			if (position==NULL) break;
		}
	}
}
	
bool CCompiler::IsKeyWord (CString mot)
{	
	return ((lexeme.Type==KEYWORD) && (lexeme.Valeur==mot));
}

void CCompiler::Send (CString text)
{
	if (EnableWriting)
	{
		text += "\r\n";
		generatedFile->Write(text);
	}
}


//////////////////////////////////////////////////////////////////////
// Fonctions auxiliaires diverses
//////////////////////////////////////////////////////////////////////


int CCompiler::TType::NbElts ()
{
	int nb_elts = 1;
	if (Tableau)		
	{
		for (int i = 1; i <= Dimension; i++)
		{
			nb_elts *= Fin[i]-Debut[i]+1;
		}
	}
	return (nb_elts);
}


int CCompiler::TType::Taille ()
{
	int nb_elts = NbElts();

	if (Nom == "STRING")
		nb_elts *= Fin[Dimension+1]-Debut[Dimension+1]+1;

	if (Indirect) return (5);
	if (Nom=="BOOLEAN") return (nb_elts);
	if (Nom=="BYTE") return (nb_elts * 2);
	if (Nom=="CHAR") return (nb_elts * 2);
	if (Nom=="INTEGER") return (nb_elts * 5);
	if (Nom=="REAL") return (nb_elts * 16);
	if (Nom=="STRING") return (nb_elts * 2);
	
	throw "Erreur interne ds la def de types:"+Nom;
}

//////////////////////////////////////////////////////////////////////
// Fonctions utiles au compilateur
//////////////////////////////////////////////////////////////////////

CString CCompiler::MaxType (CString Type1, CString Type2)
{
	if (
		((Type1 == "BYTE") && (Type2 == "INTEGER")) ||
		((Type1 == "BYTE") && (Type2 == "REAL")) ||
		((Type1 == "INTEGER") && (Type2 == "REAL")) ||
		((Type1 == "CHAR") && (Type2 == "STRING"))
		)
		return Type2; else return Type1;
}


// Compile une conversion de Type1 vers Type2 si elle est autoris�e
// ----------------------------------------------------------------

void CCompiler::CompileConversion (CString Type1, CString Type2)
{
	if (Type1 == Type2) return;

	if (
		((Type1 == "BYTE") && ((Type2 == "INTEGER") || (Type2 == "REAL")))
	 || ((Type1 == "INTEGER") && ((Type2 == "BYTE") || (Type2 == "REAL")))
	 || ((Type1 == "CHAR") && (Type2 == "STRING"))
	   )
	   Send ("CV:"+Type1+":"+Type2);
	else 
		throw "Ne peut convertir un "+Type1+" en "+Type2;
}

// Evalue l'expression en cours
// ----------------------------

CString CCompiler::Eval (int priorite = 7)
{	
	CString Type1, Type2;

	// Lit le premier op�rande (+ op�rateur unaire �ventuel)

	LireLexeme ();

	if (IsKeyWord("("))
	{
		Type1 = Eval ();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
	}
	else
	{
		if (IsKeyWord("-"))
		{
			Type1 = Eval(1);
			if ((Type1 != "BYTE") && (Type1 != "INTEGER") && (Type1 != "REAL")) throw "Nombre attendu !";
			Send ("OP:"+Type1+":NEG");
			
		}
		else
			if (IsKeyWord("NOT"))
			{
				Type1 = Eval(1);
				if (Type1 != "BOOLEAN") throw "BOOLEAN attendu !";
				Send ("OP:BOOLEAN:NEG");				
			}
			else Type1 = LireDonnee ();				
	}

	if (IsKeyWord("IN")) { Compile_In (Type1); Type1 = "BOOLEAN"; }

	// Lit le second op�rande et effectue l'op�ration binaire

	while (true)
	{
		int priorite2 = 8;

		// D�termine le niveau de priorit� de l'op�rateur binaire		
		CString op = lexeme.Valeur;
		if (lexeme.Type==KEYWORD)
		{			
			if ((op=="*") || (op=="/") || (op=="DIV") || (op=="MOD")) priorite2 = 2;
			if ((op=="+") || (op=="-")) priorite2 = 3;
			if ((op=="=") || 
				(op=="<") ||
				(op==">") ||
				(op=="<>") ||
				(op=="<=") ||
				(op==">=") ||
				(op=="=<") ||
				(op=="=>")) priorite2 = 4;
			if (op=="AND") priorite2 = 5;
			if (op=="OR") priorite2 = 6;
		}

		// Fin de traitement
		if (priorite2>=priorite) break;

		// R�cup�re le type du deuxi�me op�rande
		POSITION tmp = IndexSource;
		bool tmp2 = EnableWriting;
		EnableWriting = false;
		Type2 = Eval(priorite2);
		IndexSource = tmp;
		EnableWriting = tmp2;

		// Conversion du premier op�rande
		CompileConversion (Type1, MaxType(Type1, Type2));

		// Evalue le second op�rande
		Eval(priorite2);

		// Conversion du second op�rande
		CompileConversion (Type2, MaxType(Type1, Type2));

		// Le r�sultat est du type MaxType(Type1, Type2)
		Type1 = MaxType(Type1, Type2);

		// Gestion des types valides et des priorit�s
		if ((priorite2==5) || (priorite2==6)) if (Type1 != "BOOLEAN") throw ("BOOLEAN attendu !");
		if (priorite2==4) if ((Type1=="BOOLEAN") && ((op=="<") || (op==">") || (op=="<=") || (op==">="))) throw ("Op�rateur incompatible avec les BOOLEAN !");
		if (priorite2==3) if ((Type1=="CHAR") || ((Type1=="STRING") && (op=="-"))) throw "Op�rateur incompatible avec le type "+Type1+" ;";
		if (priorite2==2) if ((Type1 != "BYTE") && (Type1 != "INTEGER") && (Type1 != "REAL")) throw "L'op�rateur n'agit que sur des nombres !";
		if (((op=="DIV") || (op=="MOD")) && (Type1=="REAL")) throw "L'op�rateur "+op+" n'agit que sur les entiers !";
		if ((op=="/") && (Type1!="REAL")) throw "L'op�rateur / n'agit que sur les r�els !";
		
		// Effectue l'op�ration binaire
		if (Type1!="CHAR") Send("OP:"+Type1+":"+op); else Send("OP:BYTE:"+op);

		// G�n�ration de Boolean par comparaisons
		if (priorite2==4) 
		{
			Type1 = "BOOLEAN";
			priorite = 7;
		}
	}

	// retourne le type de l'expression �valu�e
	return (Type1);
}

// Est-ce une variable ?
// ---------------------

bool CCompiler::IsVariable (CString nom, CList <TVariable, TVariable&> *liste, POSITION posStart = NULL)
{	
	TVariable current;
	POSITION position = posStart;
	if (posStart==NULL) 
		position = liste->GetHeadPosition();
	else 
		liste->GetNext(position);
	while (position != NULL)
	{
		current = liste->GetNext (position);
		if (current.Nom == nom) return true;
	}
	return (false);
}

bool CCompiler::IsVariable (CString nom)
{
	return 		
		(IsVariable(nom, &VariablesStatiques) ||
		IsVariable(nom, &VariablesLocales) ||
		IsVariable(nom, &VariablesGlobales));
}

// Est-ce une proc�dure ou une fonction ?
// --------------------------------------

bool CCompiler::IsProcFun (CString nom)
{
	TProcFun current;
	POSITION position = ProceduresEtFonctions.GetHeadPosition();
	while (position != NULL)
	{
		current = ProceduresEtFonctions.GetNext (position);
		if (current.Nom == nom) return true;
	}
	return (false);
}

// Place sur la pile l'adresse de la variable en cours
// ---------------------------------------------------

CCompiler::TVariable CCompiler::newVar(CString nom, TType type, CList <TVariable, TVariable&> *liste)
{
	CCompiler::TVariable v;
	v.Nom = nom;
	v.Type = type;
	if (!liste->IsEmpty()) 
	{
		CCompiler::TVariable tmp;
		tmp=liste->GetTail();
		v.Adresse=tmp.Adresse+tmp.Type.Taille();
	}
	else 
		v.Adresse = 0;
	return v;
}

CCompiler::TType CCompiler::stringType(char longueur)
{
	CCompiler::TType tmp;
	tmp.Nom="STRING";
	tmp.Tableau=false;
	tmp.Indirect=false;
	tmp.Dimension=0;
	tmp.Debut[1]=0;
	tmp.Fin[1]=longueur;
	return tmp;
}

CString CCompiler::VariableContext (CString nom)
{
	// Variable locale ?
	if (IsVariable(nom, &VariablesLocales)) return "LOCAL";

	// Variable locale statique
	if (IsVariable(nom, &VariablesStatiques, localStatique)) return "STATIC";
	
	// Variable globale ?
	if (IsVariable(nom, &VariablesGlobales)) return "GLOBAL";

	// Variable globale statique
	if (IsVariable(nom, &VariablesStatiques)) return "STATIC";

	throw "Erreur interne, la variable "+nom+" est recherch�e mais n'existe pas !";
}


CCompiler::TVariable CCompiler::VariableNamed (CString nom)
{
	CList <TVariable, TVariable&> *liste;
	POSITION position = NULL;

	// Variable locale ?
	if (IsVariable(nom, &VariablesLocales)) liste = &VariablesLocales;

	// Variable locale statique
	else if (IsVariable(nom, &VariablesStatiques, localStatique))
	{
		liste = &VariablesStatiques;
		position = localStatique;
	}

	// Variable globale ?
	else if (IsVariable(nom, &VariablesGlobales)) liste = &VariablesGlobales;

	// Variable globale statique
	else if (IsVariable(nom, &VariablesStatiques)) liste = &VariablesStatiques;

	else throw "Erreur interne, la variable "+nom+" est recherch�e mais n'existe pas !";

	// On d�marre la recherche de la variable apr�s la position posStart

	if (position==NULL)
		position = liste->GetHeadPosition();
	else
		liste->GetNext(position);

	// On effectue la recherche
	TVariable current = liste->GetAt(position);
	while (current.Nom != nom) current = liste->GetNext (position);

	return current;
}


CString CCompiler::AccesVariable ()
{
	// R�cup�re la variable et son contexte m�moire
	TVariable current = VariableNamed(lexeme.Valeur);
	CString Adresse = VariableContext(lexeme.Valeur);

	// On place l'adresse de base sur la pile
	Send("CST:"+Adresse+":"+IntToStr(current.Adresse));

	// Gestion de l'acc�s indirect (par pointeur)
	if (current.Type.Indirect) Send ("LOAD:INTEGER");


	// Gestion des tableaux
	// --------------------

	if (current.Type.Tableau)
	{
		LireLexeme ();
		if (!IsKeyWord("[")) throw "[ attendu !";

		// On calcule le d�calage en nombre d'�l�ments
		for (int i=1; i<=current.Type.Dimension; i++)
		{
			if (i>1)
			{
				Send ("CST:INTEGER:"+IntToStr(current.Type.Fin[i]-current.Type.Debut[i]+1));
				Send ("OP:INTEGER:*");
				if (!IsKeyWord(","))
					if (IsKeyWord("]"))
					{
						LireLexeme();
						if (!IsKeyWord("[")) throw "[ attendu !";
					}
					else throw ", attendu !";
			}
			CString typ = Eval ();
			CompileConversion (typ, "INTEGER");
			Send ("CST:INTEGER:"+IntToStr(current.Type.Debut[i]));
			Send ("OP:INTEGER:-");
			if (i>1) Send ("OP:INTEGER:+");
		}
		if (!IsKeyWord("]")) throw "] attendu !";

		// On place sur la pile la taille des �l�ments du tableau
		if (current.Type.Nom=="BOOLEAN") Send ("CST:INTEGER:1");
		if (current.Type.Nom=="CHAR") Send ("CST:INTEGER:2");
		if (current.Type.Nom=="BYTE") Send ("CST:INTEGER:2");
		if (current.Type.Nom=="INTEGER") Send ("CST:INTEGER:5");
		if (current.Type.Nom=="REAL") Send ("CST:INTEGER:16");

		if (current.Type.Nom=="STRING")
		{
			LireLexeme ();
			if (IsKeyWord("["))
			{
				Send ("CST:INTEGER:"+IntToStr(current.Type.Fin[current.Type.Dimension+1]-current.Type.Debut[current.Type.Dimension+1]+1));
				Send ("OP:INTEGER:*");
				CString typ = Eval ();
				CompileConversion (typ, "INTEGER");
				Send ("CST:INTEGER:"+IntToStr(current.Type.Debut[current.Type.Dimension+1]));
				Send ("OP:INTEGER:-");
				Send ("OP:INTEGER:+");
				Send ("CST:INTEGER:2");
				Send ("OP:INTEGER:*");
				Send ("OP:INTEGER:+");
				if (!IsKeyWord("]")) throw "] attendu !";			
				return ("CHAR");
			}
			else FichierEntree->lexemes.GetPrev (IndexSource);
			Send ("CST:INTEGER:"+IntToStr(current.Type.Fin[current.Type.Dimension+1]-current.Type.Debut[current.Type.Dimension+1]+1));
			Send ("OP:INTEGER:*");
			Send ("CST:INTEGER:2");
		}

		// On ajoute, � l'adresse de base, le d�calage en quartets
		Send ("OP:INTEGER:*");
		Send ("OP:INTEGER:+");
	}
	else
	{
		// Gestion de l'acc�s aux caract�res d'une chaine
		if (current.Type.Nom=="STRING")
		{
			LireLexeme ();
			if (IsKeyWord("["))
			{			
				CString typ = Eval ();
				CompileConversion (typ, "INTEGER");
				Send ("CST:INTEGER:"+IntToStr(current.Type.Debut[current.Type.Dimension+1]));
				Send ("OP:INTEGER:-");
				Send ("CST:INTEGER:2");
				Send ("OP:INTEGER:*");
				Send ("OP:INTEGER:+");
				if (!IsKeyWord("]")) throw "] attendu !";			
				return ("CHAR");
			} else FichierEntree->lexemes.GetPrev (IndexSource);
		}
	}

	// Retourne le type de la variable lue
	return (current.Type.Nom);
}

// Charge une donn�e sur la pile
// -----------------------------

CString CCompiler::LireDonnee()
{
	// Conversions
	if (IsKeyWord("CHR"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "BYTE");		
		return ("CHAR");
	}

	if (IsKeyWord("ORD"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "CHAR");		
		return ("BYTE");
	}

	if (IsKeyWord("ROUND"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "REAL");
		Send ("CV:REAL:INTEGER");
		return ("INTEGER");
	}

	if (IsKeyWord("REALTOSTR"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "REAL");
		Send ("CV:REAL:STRING");
		return ("STRING");
	}

	if (IsKeyWord("INTTOSTR"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "INTEGER");
		Send ("CV:INTEGER:STRING");
		return ("STRING");
	}

	if (IsKeyWord("STRTOINT"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "STRING");
		Send ("CV:STRING:INTEGER");
		return ("INTEGER");
	}

	if (IsKeyWord("STRTOREAL"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "STRING");
		Send ("CV:STRING:REAL");
		return ("REAL");
	}

	if (IsKeyWord("LENGTH"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ = Eval();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		CompileConversion (typ, "STRING");
		Send ("OP:STRING:LENGTH");
		return ("BYTE");
	}

	if (IsKeyWord("COPY"))
	{
		LireLexeme ();
		if (!IsKeyWord("(")) throw "( attendu !";
		CString typ1 = Eval();
		CompileConversion (typ1, "STRING");
		if (!IsKeyWord(",")) throw ", attendu !";
		CString typ2 = Eval();
		CompileConversion (typ2, "BYTE");
		if (!IsKeyWord(",")) throw ", attendu !";
		CString typ3 = Eval();
		CompileConversion (typ3, "BYTE");
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
		Send ("OP:STRING:COPY");
		return ("STRING");
	}


	// Chargement des constantes
	if (lexeme.Type==ENTIER)
	{
		Send("CST:INTEGER:"+lexeme.Valeur);
		LireLexeme ();
		return ("INTEGER");
	}

	if (lexeme.Type==FLOTTANT)
	{
		Send("CST:REAL:"+lexeme.Valeur);
		LireLexeme ();
		return ("REAL");
	}

	if (lexeme.Type==CARACTERE)
	{
		Send("CST:BYTE:"+IntToStr(lexeme.Valeur[0]));
		LireLexeme ();
		return ("CHAR");
	}

	if (lexeme.Type==CHAINE)
	{
		if (EnableWriting)
		{
			TType t=stringType(lexeme.Valeur.GetLength());
			TVariable v = newVar("@@", t, &VariablesStatiques);
			VariablesStatiques.AddTail(v);
								
			Send ("CONST:STRING:"+IntToStr(lexeme.Valeur.GetLength())); 
			Send ("DATA:"+lexeme.Valeur);
			Send ("ENDCONST");
			Send ("CST:STATIC:"+IntToStr(v.Adresse));
			Send ("LOAD:STRING");
		}
		LireLexeme ();
		return ("STRING");
	}

	if (IsKeyWord("TRUE"))
	{
		Send("CST:BOOLEAN:"+lexeme.Valeur);
		LireLexeme ();
		return ("BOOLEAN");
	}
	
	if (IsKeyWord("FALSE"))
	{
		Send("CST:BOOLEAN:"+lexeme.Valeur);
		LireLexeme ();
		return ("BOOLEAN");
	}	
	
	// Chargement des variables
	if (IsKeyWord("RESULT"))
	{
		if (ProcName=="") throw "Ce mot cl� est r�serv� � une utilisation dans les fonctions !";
		lexeme.Type=ID;
		lexeme.Valeur=ProcName;
	}

	// lexeme = mot cl� ou identificateur
	if (IsVariable(lexeme.Valeur))
	{		
		CString typ = AccesVariable();
		LireLexeme ();
		if (typ!="CHAR")
			Send ("LOAD:"+typ);
		else
			Send ("LOAD:BYTE");
		return (typ);
	}

	// Chargement des fonctions
	if (IsProcFun(lexeme.Valeur))
	{
		CString typ = CompileAppelFonction();
		if (typ=="NIL") throw "Cette fonction ne renvoie pas de r�sultat !";
		return (typ);
	}

	throw "Identificateur attendu !";
}

//////////////////////////////////////////////////////////////////////
// Le compilateur se trouve ici
//////////////////////////////////////////////////////////////////////


// Compile la directive VAR
// ------------------------

void CCompiler::CompileVar (CList <TVariable, TVariable&> *ListeVariables, bool Fonc = false, bool Constante = false)
{	
	bool Indirect = false;
	LireLexeme ();

	while ((lexeme.Type == ID) || (IsKeyWord("VAR") && (Fonc)))
	{
		if (IsKeyWord("VAR"))
		{			
			Indirect = true;
			LireLexeme ();
		}

		// Lit la liste des variables d�clar�es dans une ligne
		CList <CString, CString&> ListeNoms;
		ListeNoms.RemoveAll();
		int ListeVarCount = 0;

		while (true)
		{
			if (lexeme.Type!=ID) throw "Identificateur attendu !";
			if (IsProcFun(lexeme.Valeur)) throw "Identificateur red�clar� !";
			
			if (IsVariable(lexeme.Valeur))
			{
				// Si on est pas dans un contexte local
				if ((ListeVariables==&VariablesGlobales)
					|| ((ListeVariables==&VariablesStatiques) && (localStatique==NULL)))
					throw "Identificateur red�clar� !";

				// Si (contexte local) la variable existe d�j� en local
				if ((IsVariable(lexeme.Valeur, &VariablesLocales))
					|| (IsVariable(lexeme.Valeur, &VariablesStatiques, localStatique)))
					throw "Identificateur red�clar� !";

				// Si la constante non typ�e existe d�j�
				{
					POSITION pos = Constantes.GetHeadPosition();
					while (pos!=NULL)
					{
						TCste tmp=Constantes.GetNext(pos);
						if (lexeme.Valeur==tmp.nom) throw "Constante d�j� d�clar�e";
					}	
				}

			}

			if (Constante) NomVariablesConstantes.AddTail(lexeme.Valeur);

			ListeNoms.AddTail(lexeme.Valeur);
			ListeVarCount++;
			LireLexeme ();
			if (!IsKeyWord(",")) break;
			LireLexeme ();
			if (lexeme.Type != ID) throw "Identificateur attendu !";
		}

		// Affectation des constantes non typ�es
		if (IsKeyWord("="))
		{
			LireLexeme();
			if (ListeNoms.GetCount()>1) throw "Les constantes non typ�es doivent �tre d�clar�es une par une";
			if (lexeme.Type==KEYWORD) throw "Vous ne pouvez pas affecter de mot r�serv� � une constante !";
			if (lexeme.Type==ID) throw "Vous ne pouvez pas affecter d'identificateur � une constante !";
			if (lexeme.Type==ASSEMBLER) throw "Vous ne pouvez pas affecter de mot r�serv� � une constante !";
			TCste tmp;
			tmp.nom=ListeNoms.GetHead();
			tmp.valeur.Type = lexeme.Type;
			tmp.valeur.Valeur = lexeme.Valeur;			
			Constantes.AddTail(tmp);
			LireLexeme();
			if (!IsKeyWord(";")) throw "; attendu !";
			LireLexeme ();
			continue;
		}

		if (!IsKeyWord(":")) throw ": attendu!";
		LireLexeme();

		// R�cup�re le type de ces variables
		TType Type;
		
		Type.Indirect = Indirect;

		if (IsKeyWord("ARRAY"))
		{
			// Tableaux
			if (Fonc) throw "Ne peut pas passer en param�tre un tableau !";
			Type.Tableau = true;
			Type.Dimension = 0;

			LireLexeme ();
			if (!IsKeyWord("[")) throw "[ attendu !";

			while (true)
			{
				Type.Dimension ++;
				if (Type.Dimension > 3) throw "Un tableau doit �tre de dimension au plus 3 !";

				LireLexeme ();
				if (lexeme.Type!=ENTIER) throw "Nombre entier attendu !";
				Type.Debut[Type.Dimension] = StrToInt(lexeme.Valeur.GetString());
				LireLexeme ();
				if (!IsKeyWord("..")) throw ".. attendu !";
				LireLexeme ();
				if (lexeme.Type!=ENTIER) throw "Nombre entier attendu !";
				Type.Fin[Type.Dimension] = StrToInt(lexeme.Valeur.GetString());
				LireLexeme ();
				if (!IsKeyWord(","))
				{
					if (!IsKeyWord("]")) throw "] attendu !";
					LireLexeme ();
					if (IsKeyWord("OF")) break;
					if (!IsKeyWord("[")) throw "OF attendu !";
				}
			}
			LireLexeme ();
		}
		else
		{
			Type.Tableau = false;
			Type.Dimension = 0;
		}

		if ((IsKeyWord("BYTE")) ||
			(IsKeyWord("INTEGER")) ||
			(IsKeyWord("REAL")) ||
			(IsKeyWord("CHAR")) ||
			(IsKeyWord("STRING")) ||
			(IsKeyWord("BOOLEAN")))
		{
			Type.Nom = lexeme.Valeur;
			if (IsKeyWord("STRING"))
			{
				if (Type.Dimension == 3) throw "Un tableau de STRING doit �tre de dimension au plus 2 !";				
				Type.Debut[Type.Dimension+1] = 0;
				Type.Fin[Type.Dimension+1] = 127;
				LireLexeme();
				if (IsKeyWord("["))
				{
					LireLexeme();
					if (lexeme.Type!=ENTIER) throw "Nombre entier attendu !";
					Type.Fin[Type.Dimension+1] = StrToInt(lexeme.Valeur.GetString());
					LireLexeme();
					if (!IsKeyWord("]")) throw "] attendu !";
					LireLexeme();
				}
			}
			else
				LireLexeme ();
		}
		else throw "Type attendu !";


		// Stocke en m�moire toutes ces variables
		while (!ListeNoms.IsEmpty())
		{
			TVariable var;
			var.Nom = ListeNoms.RemoveHead();
			var.Type = Type;
			if (!ListeVariables->IsEmpty()) var.Adresse = ListeVariables->GetTail().Adresse + ListeVariables->GetTail().Type.Taille();
			else var.Adresse = 0;

			ListeVariables->AddTail (var);
		}

		// Gestion des constantes (et variables statiques)
		if (ListeVariables==&VariablesStatiques)
		{
			if (IsKeyWord("="))
			{
				if ((ListeVariables==&VariablesGlobales) || (ListeVariables==&VariablesLocales))
					throw "Vous ne pouvez d�finir que les constantes ou les variables statiques !";
				if (Type.Nom=="CHAR") Send ("CONST:BYTE");
				else if (Type.Nom=="STRING") Send ("CONST:STRING:"+IntToStr(Type.Fin[Type.Dimension+1]));
				else Send ("CONST:"+Type.Nom);
				for (int rien=1; rien<=ListeVarCount; rien++)
				{
					for (int indice = 1; indice<=Type.NbElts(); indice++)
					{				
						if (indice!=1) if (!IsKeyWord(",")) throw ", attendue !";
						LireLexeme ();

						// V�rification du typage de la constante
						if ((Type.Nom=="CHAR") && (lexeme.Type!=CARACTERE)) throw "caract�re attendu !";
						if ((Type.Nom=="STRING") && (lexeme.Type!=CHAINE) && (lexeme.Type!=CARACTERE)) throw "chaine de caract�re attendue !";
						if (((Type.Nom=="BYTE") || (Type.Nom=="INTEGER")) && (lexeme.Type!=ENTIER)) throw "entier attendu !";
						if ((Type.Nom=="REAL") && (lexeme.Type!=ENTIER) && (lexeme.Type!=FLOTTANT)) throw "nombre entier/flottant attendu !";

						if (Type.Nom!="CHAR") 
							Send ("DATA:"+lexeme.Valeur);
						else
							Send ("DATA:"+IntToStr(lexeme.Valeur[0]));
						LireLexeme ();
					}
					if ((rien<ListeVarCount) && (!IsKeyWord(";"))) throw "; attendu !";
				}
				Send ("ENDCONST");
			}
			else
			{
				if (Type.Nom=="CHAR") Send ("CONST:BYTE");
				else if (Type.Nom=="STRING") Send ("CONST:STRING:"+IntToStr(Type.Fin[Type.Dimension+1]));
				else Send ("CONST:"+Type.Nom); 
				for (int rien=1; rien<=ListeVarCount; rien++)
				{
					for (int indice = 1; indice<=Type.NbElts(); indice++)
					{									
						int prova = Type.NbElts();

						if ((Type.Nom=="BYTE") || (Type.Nom=="CHAR") || (Type.Nom=="INTEGER") || (Type.Nom=="REAL"))
							Send ("DATA:0");
						if (Type.Nom=="BOOLEAN")
							Send ("DATA:FALSE");
						if (Type.Nom=="STRING")
							Send ("DATA:");
					}
				}
				Send ("ENDCONST");
			}
		}

		// Passe aux d�clarations suivantes
		if (!Fonc)
		{
			if (!IsKeyWord(";")) throw "; attendu ;";
			LireLexeme();
		}

		if (Fonc)
		{
			if (IsKeyWord(")")) return;
			if (!IsKeyWord(";")) throw "; attendu !";
			LireLexeme ();
		}
	}

	if (Fonc) throw "Identificateur attendu !";

}

// Compile les proc�dures et fonctions
// -----------------------------------


bool CCompiler::sameFonction (CCompiler::TProcFun f1, CCompiler::TProcFun f2)
{
	if (f1.Nom!=f2.Nom) return false;
	if (f1.Resultat.Nom!=f2.Resultat.Nom) return false;
	
	POSITION p1 = f1.PremiereVariable;
	POSITION p2 = f2.PremiereVariable;

	if ((p1==NULL) && (p2==NULL)) return true;
	if ((p1==NULL) || (p2==NULL)) return false;

	while (true)
	{
		TVariable v1=VariablesParametres.GetAt(p1);
		TVariable v2=VariablesParametres.GetAt(p2);

		if (v1.Type.Nom!=v2.Type.Nom) return false;		

		if ((p1==f1.DerniereVariable) && (p2==f2.DerniereVariable)) break;
		if ((p1==f1.DerniereVariable) || (p2==f2.DerniereVariable)) return false;

		VariablesParametres.GetNext(p1);
		VariablesParametres.GetNext(p2);
	}

	return true;
}


bool CCompiler::isFonction (CCompiler::TProcFun fonc)
{
	POSITION position = ProceduresEtFonctions.GetHeadPosition();
	if (position==NULL) return false;

	while (true)
	{
		TProcFun current = ProceduresEtFonctions.GetNext (position);
		if (sameFonction(current, fonc)) return true;	
		if (position==NULL) break;
	}
	return false;
}



void CCompiler::CompileProcFun()
{
	int SizeParameterMemory;
	int SizeLocalMemory;
	int LastAdresse;

	TProcFun temp;
	if (IsKeyWord("PROCEDURE"))
		temp.Resultat.Nom = "NIL";
	else
		temp.Resultat.Nom = "WAIT";
	
	LireLexeme();
	if (lexeme.Type!=ID) throw "Identificateur attendu !";
	if (IsVariable(lexeme.Valeur)) throw "Identificateur d�j� utilis� !";

	temp.Nom = lexeme.Valeur;

	if (IsProcFun(lexeme.Valeur)) 
	{	
		TProcFun last;
		POSITION position = ProceduresEtFonctions.GetTailPosition();
		last = ProceduresEtFonctions.GetAt(position);
		while (last.Nom != lexeme.Valeur) last = ProceduresEtFonctions.GetPrev (position);
		temp.version = last.version+1;		
	}
	else 
		temp.version = 1;

	VariablesLocales.RemoveAll();

	temp.PremiereVariable = NULL;
	temp.DerniereVariable = NULL;

	LireLexeme();
	if ((!IsKeyWord(";")) && (!IsKeyWord(":")))
	{
		if (!IsKeyWord("(")) throw "( ou ; attendu !";
		temp.PremiereVariable = VariablesParametres.GetTailPosition();
		CompileVar (&VariablesLocales, true);
		VariablesParametres.AddTail(&VariablesLocales);
		if (temp.PremiereVariable!=NULL) 
			VariablesParametres.GetNext(temp.PremiereVariable);
		else
			temp.PremiereVariable=VariablesParametres.GetHeadPosition();
		temp.DerniereVariable=VariablesParametres.GetTailPosition();
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
	}
		
	if (!(VariablesLocales.IsEmpty()))
	{
		TVariable var = VariablesLocales.GetTail ();
		SizeParameterMemory = var.Adresse + var.Type.Taille();
	} else SizeParameterMemory = 0;
	
	if (temp.Resultat.Nom == "NIL") 
		if (!IsKeyWord(";")) throw "; attendu !";

	if (temp.Resultat.Nom == "WAIT") 
	{
		if (!IsKeyWord(":")) throw ": attendu !";
		LireLexeme ();
		if (IsKeyWord("ARRAY")) throw "Ne peut pas renvoyer un tableau !";
		if ((!IsKeyWord("BYTE")) && (!IsKeyWord("CHAR")) && (!IsKeyWord("REAL")) &&
			(!IsKeyWord("INTEGER")) && (!IsKeyWord("STRING")) && (!IsKeyWord("BOOLEAN"))) throw "Type attendu !";
		temp.Resultat.Tableau = false;
		temp.Resultat.Nom = lexeme.Valeur;
		if (IsKeyWord("STRING"))
		{
			temp.Resultat.Debut[1] = 0;
			temp.Resultat.Fin[1] = 127;
			temp.Resultat.Dimension = 0;
		}
		LireLexeme();
		if (!IsKeyWord(";")) throw "; attendu !";
		temp.Resultat.Indirect=false;

		// Ajoute la variables contenant le r�sultat
		{
		TVariable var;
		var.Nom=temp.Nom;
		var.Adresse=0;
		var.Type=temp.Resultat;
		VariablesLocales.AddHead (var);
		}
		POSITION position = VariablesLocales.GetHeadPosition ();
		VariablesLocales.GetNext (position);
		while (position!=NULL)
		{
			TVariable var = VariablesLocales.GetAt(position);
			var.Adresse += temp.Resultat.Taille();
			VariablesLocales.SetAt(position, var);
			VariablesLocales.GetNext (position);
		}
	}

	// M�morise la d�claration de la proc�dure
	if (isFonction(temp)) throw "Fonction d�j� d�clar�e avec les m�mes param�tres !";
	ProceduresEtFonctions.AddTail (temp);
	LireLexeme ();

	int SizeResultatMemory;

	if (temp.Resultat.Nom=="NIL") 
		SizeResultatMemory = 0;
	else 
		SizeResultatMemory = temp.Resultat.Taille();

	localStatique = VariablesStatiques.GetTailPosition();
	localCste = NomVariablesConstantes.GetTailPosition();

	while (true)
	{
		// Gestion des variables locales
		if (IsKeyWord("VAR")) CompileVar(&VariablesLocales);
		
		// Gestion des constantes/statiques ... locales !
		else if (IsKeyWord("STATIC")) CompileVar(&VariablesStatiques);
		else if (IsKeyWord("CONST")) CompileVar(&VariablesStatiques, false, true);

		else break;
	}

	if (!(VariablesLocales.IsEmpty()))
	{		
		TVariable var = VariablesLocales.GetTail ();
		LastAdresse = var.Adresse + var.Type.Taille();
		if (temp.Resultat.Nom=="NIL") 
			SizeLocalMemory = var.Adresse + var.Type.Taille() - SizeParameterMemory;
		else 
			SizeLocalMemory = var.Adresse + var.Type.Taille() - SizeParameterMemory - temp.Resultat.Taille();
	}
	else 
	{
		SizeLocalMemory = 0;
		LastAdresse = 0;
	}		
	
	// Gestion de l'en-t�te proc�dural
	if (!IsKeyWord("BEGIN")) throw "BEGIN attendu !";
	ProcName = temp.Nom;
	ProcVersion = temp.version;
	Send ("PROC:"+ProcName+"@"+IntToStr(temp.version)+":"+IntToStr(SizeResultatMemory)+":"+IntToStr(SizeParameterMemory)+":"+IntToStr(SizeLocalMemory));
	
	// Les instructions
	LabelBreak = NextLabel();
	CompileInstrs ();
	Send ("LABEL:"+LabelBreak);
	LabelBreak = "";

	// Fin de la proc�dure
	Send ("ENDPROC");
	ProcName = "";
	
	// Gestion des variables locales
	VariablesLocales.RemoveAll();

	// Gestion des constantes/statiques ... locales !
	POSITION position = localStatique;
	while (position!=VariablesStatiques.GetTailPosition())
	{
		VariablesStatiques.GetNext(position);
		TVariable tmp = VariablesStatiques.GetAt(position);
		tmp.Nom="@";
		VariablesStatiques.SetAt(position, tmp);
	}
	position = localCste;
	while (position!=NomVariablesConstantes.GetTailPosition())
	{
		NomVariablesConstantes.GetNext(position);
		NomVariablesConstantes.SetAt(position, (CString)"@");
	}
	localStatique = NULL;
	localCste = NULL;
	
	if (!IsKeyWord(";")) throw "; attendu !";
	LireLexeme ();
}


// Compile la directive IN [?..?,?,?]
// ----------------------------------

void CCompiler::Compile_In (CString type)
{
	// Typage
	if ((type=="REAL") || (type=="STRING")) throw "Le type "+type+" n'est pas autoris� avec IN !";

	// Les CHAR sont g�r�s en interne comme des BYTE
	CString typeReel=type;
	if (type=="CHAR") typeReel = "BYTE";

	LireLexeme ();
	if (!IsKeyWord("[")) throw "[ attendu !";
	LireLexeme ();

	bool flag = false;
	CString l1 = NextLabel ();
	CString l2 = NextLabel ();
	while (!flag)
	{
		if ((lexeme.Type!=ENTIER) && ((type=="BYTE") || (type=="INTEGER"))) throw "Nombre entier attendu !";
		if ((lexeme.Type!=CARACTERE) && (type=="CHAR")) throw "Caract�re attendu !";
		if ((!IsKeyWord("TRUE")) && (!IsKeyWord("FALSE")) && (type=="BOOLEAN")) throw "BOOLEAN attendu !";
		if (type!="CHAR")
			Send ("CST:"+type+":"+lexeme.Valeur);
		else
			Send ("CST:BYTE:"+IntToStr(lexeme.Valeur[0]));
		LireLexeme ();
		if (IsKeyWord("]") || IsKeyWord(",")) { Send ("GO_IF_EQUAL:"+typeReel+":"+l1); }
		else if (IsKeyWord(".."))
		{					
			LireLexeme ();				
			if ((lexeme.Type!=ENTIER) && ((type=="BYTE") || (type=="INTEGER"))) throw "Nombre entier attendu !";
			if ((lexeme.Type!=CARACTERE) && (type=="CHAR")) throw "Caract�re attendu !";
			if (type=="BOOLEAN") throw ".. interdit avec des BOOLEANs !";
			if (type!="CHAR")
				Send ("CST:"+type+":"+lexeme.Valeur);
			else
				Send ("CST:BYTE:"+IntToStr(lexeme.Valeur[0]));
			Send ("GO_IF_IN:"+typeReel+":"+l1);
			LireLexeme ();
			if ((!IsKeyWord("]")) && (!IsKeyWord(","))) throw "] attendu !";
		}
		else throw "] attendu !";
		if (IsKeyWord("]")) flag = true;
		LireLexeme ();
	}
	Send ("DROP:"+typeReel);
	Send ("CST:BOOLEAN:FALSE");
	Send ("GOTO:"+l2);
	Send ("LABEL:" + l1);
	Send ("DROP:"+typeReel);
	Send ("CST:BOOLEAN:TRUE");
	Send ("LABEL:" + l2);
}


// Compile une instruction
// -----------------------

void CCompiler::CompileInstrs ()
{
	// Permet de compiler un bloc BEGIN/END

	if (IsKeyWord("BEGIN")) 
	{		
		LireLexeme ();
		while (true)
		{
			CompileInstr ();
			if (IsKeyWord("END")) break;
			if (!IsKeyWord(";")) throw "; attendu !";
			LireLexeme ();
		}
		LireLexeme ();
	}
	else
		CompileInstr ();
}

void CCompiler::CompileInstr ()
{
	// Pas d'instruction
	if (IsKeyWord("END") || IsKeyWord(";")) return;
	
	// Toutes les instructions possibles sont trait�es ici

	if (lexeme.Type==ASSEMBLER)
	{
		while (lexeme.Type==ASSEMBLER)
		{
			Send ((CString)"REM:Ligne "+IntToStr(lexeme.Ligne));
			if (lexeme.Valeur.Find((CString) "@")!=-1) throw "Caract�re @ interdit !";
			switch (lexeme.Valeur[0])
			{
			case ':' : Send ((CString)"LABEL:@A@"+ProcName+"@"+IntToStr(ProcVersion)+"@"+lexeme.Valeur.Right(lexeme.Valeur.GetLength()-1)); break;
			case 'G' :
				{
					CString Appel, Adr;
					int separateur;
					separateur=lexeme.Valeur.Find(' ');
					Appel = lexeme.Valeur.Left(separateur);
					Adr = lexeme.Valeur.Mid(separateur, lexeme.Valeur.GetLength()-separateur);
					Adr.TrimLeft(' ');
					Adr.TrimRight(' ');
					if ((Adr[0]>='A') && (Adr[0]<='Z'))
						Send ((CString)"ASM:"+Appel+" @A@"+ProcName+"@"+IntToStr(ProcVersion)+"@"+Adr);
					else 
						Send ((CString)"ASM:"+Appel+" "+Adr);
					break;
				}
			default:
				if ((lexeme.Valeur.Left(9)=="A=OFFSET ") || (lexeme.Valeur.Left(9)=="C=OFFSET "))
				{
					// R�cup�re le nom de la variable
					CString var = lexeme.Valeur.Mid(9, lexeme.Valeur.GetLength()-9);					
					var.TrimLeft(32);
					var.TrimRight(32);
					
					if (!IsVariable(var)) throw (CString)"Variable attendue, "+var+" trouv� ...";
					
					// Recherche la variable
					TVariable current = VariableNamed(var);

					// On place l'adresse de base sur la pile
					Send((CString)"ASM:L"+lexeme.Valeur[0]+"(5) "+IntToStr(current.Adresse));
				}
				else
					Send((CString)"ASM:"+lexeme.Valeur);

			}
			LireLexeme ();
		}
		return;
	}

	if (IsKeyWord("IF"))
	{
		CString l1 = NextLabel();
		CString tmp=NextLabel();
		if (Eval()!="BOOLEAN") throw "BOOLEAN attendu";
		if (!IsKeyWord("THEN")) throw "THEN attendu !";
		LireLexeme ();
		Send("GOYES:"+tmp);
		Send("GOLONG:"+l1);
		Send("LABEL:"+tmp);
		CompileInstrs ();
		if (IsKeyWord(";"))
			Send("LABEL:"+l1);
		if (IsKeyWord("ELSE"))
		{
			CString l2 = NextLabel();
			Send("GOLONG:"+l2);
			Send("LABEL:"+l1);
			LireLexeme ();
			CompileInstrs ();
			Send("LABEL:"+l2);
		}
		return;
	}

	if (IsKeyWord("WHILE"))
	{
		CString l1 = NextLabel();
		CString l2 = NextLabel();
		CString tmp1 = LabelBreak, tmp2 = LabelContinue;
		LabelBreak = l2; LabelContinue = l1;
		Send ("LABEL:"+l1);
		if (Eval()!="BOOLEAN") throw "BOOLEAN attendu !";
		if (!IsKeyWord("DO")) throw "DO attendu !";
		LireLexeme ();
		CString l3 = NextLabel();
		Send("GOYES:"+l3);
		Send("GOLONG:"+l2);
		Send("LABEL:"+l3);
		CompileInstrs ();
		Send("GOLONG:"+l1);
		Send("LABEL:"+l2);
		LabelBreak = tmp1; LabelContinue = tmp2;
		return;
	}

	if (IsKeyWord("REPEAT"))
	{
		CString l1 = NextLabel();
		CString l2 = NextLabel();
		CString tmp1 = LabelBreak, tmp2 = LabelContinue;
		LabelBreak = l2; LabelContinue = l1;
		Send ("LABEL:"+l1);
		LireLexeme ();
		if (!IsKeyWord("UNTIL")) while (true)
		{
			CompileInstr ();
			if (!IsKeyWord(";")) throw "; attendu !";
			LireLexeme ();
			if (IsKeyWord("UNTIL")) break;
		}
		if (Eval()!="BOOLEAN") throw "BOOLEAN attendu";
		Send("GOYES:"+l2);
		Send("GOLONG:"+l1);
		Send("LABEL:"+l2);
		LabelBreak = tmp1; LabelContinue = tmp2;
		return;
	}

	if (IsKeyWord("FOR"))
	{
		CString l1 = NextLabel();
		CString l2 = NextLabel();
		CString tmp1 = LabelBreak, tmp2 = LabelContinue;
		LabelBreak = l2; LabelContinue = l1;
		LireLexeme ();
		if (!(IsVariable(lexeme.Valeur))) throw "Variable attendue !";
		CString Typ = AccesVariable ();
		if ((Typ!="BYTE") && (Typ!="INTEGER")) throw "Variable enti�re attendue !";
		Send ("DUP:INTEGER");
		LireLexeme ();
		if (!IsKeyWord(":=")) throw ":= attendu !";
		CString Typtmp = Eval();
		CompileConversion (Typtmp, Typ);
		Send ("SAVE:"+Typ);		
		Send ("LABEL:"+l1);
		Send ("DUP:INTEGER");
		Send ("LOAD:"+Typ);
		if ((!IsKeyWord("TO")) && (!IsKeyWord("DOWNTO"))) throw "TO/DOWNTO attendu !";
		bool flag = true;
		if (IsKeyWord("DOWNTO")) flag=false;
		CString Typtmp2 = Eval();
		CompileConversion (Typtmp2, Typ);
		if (flag) Send ("OP:"+Typ+":<=");
		if (!flag) Send ("OP:"+Typ+":>=");
		CString tmp=NextLabel();
		Send ("GOYES:"+tmp);
		Send ("GOLONG:"+l2);
		Send ("LABEL:"+tmp);
		if (!IsKeyWord("DO")) throw "DO attendu !";
		LireLexeme ();
		CompileInstrs ();
		Send ("DUP:INTEGER");
		Send ("DUP:INTEGER");
		Send ("LOAD:INTEGER");
		Send ("CST:INTEGER:1");
		if (flag) Send ("OP:INTEGER:+");
		if (!flag) Send ("OP:INTEGER:-");
		Send ("SAVE:INTEGER");
		Send ("GOLONG:"+l1);
		Send ("LABEL:"+l2);
		Send ("DROP:INTEGER");
		LabelBreak = tmp1; LabelContinue = tmp2;
		return;
	}

	if (IsKeyWord("CASE"))
	{
		CString l3 = NextLabel ();
		CString typ = Eval ();
		if ((typ=="STRING") || (typ=="REAL")) throw "Le type "+typ+" n'est pas accept� !";
		if (!IsKeyWord("OF")) throw "OF attendu !";
		LireLexeme ();
		while ((!IsKeyWord("END")) && (!IsKeyWord("ELSE")))
		{
			bool flag = false;
			CString l1 = NextLabel ();
			CString l2 = NextLabel ();
            while (!flag)
			{
				if ((lexeme.Type!=ENTIER) && (typ=="BYTE")) throw "Nombre entier attendu !";
				if ((lexeme.Type!=ENTIER) && (typ=="INTEGER")) throw "Nombre entier attendu !";
				if ((lexeme.Type!=CARACTERE) && (typ=="CHAR")) throw "Caract�re attendu !";
				if ((!IsKeyWord("TRUE")) && (!IsKeyWord("FALSE")) && (typ=="BOOLEAN")) throw "BOOLEAN attendu !";
				if (typ!="CHAR")
					Send ("CST:"+typ+":"+lexeme.Valeur);
				else
					Send ("CST:BYTE:"+IntToStr(lexeme.Valeur[0]));
				LireLexeme ();
				if ((IsKeyWord(":")) || (IsKeyWord(",")))
				{                   
					if (typ!="CHAR") Send ("GO_IF_EQUAL:"+typ+":"+l1); else Send ("GO_IF_EQUAL:BYTE:"+l1);
				}
				else if (IsKeyWord(".."))
				{					
                    LireLexeme ();				
					if ((lexeme.Type!=ENTIER) && (typ=="BYTE")) throw "Nombre entier attendu !";
					if ((lexeme.Type!=ENTIER) && (typ=="INTEGER")) throw "Nombre entier attendu !";
					if ((lexeme.Type!=CARACTERE) && (typ=="CHAR")) throw "Caract�re attendu !";
					if (typ=="BOOLEAN") throw ".. interdit avec des BOOLEANs !";
					if (typ!="CHAR")
						Send ("CST:"+typ+":"+lexeme.Valeur);
					else
						Send ("CST:BYTE:"+IntToStr(lexeme.Valeur[0]));
                    if (typ!="CHAR") Send ("GO_IF_IN:"+typ+":"+l1); else Send ("GO_IF_IN:BYTE:"+l1);
					LireLexeme ();
                    if ((!IsKeyWord(":")) && (!IsKeyWord(","))) throw ": attendu !";
				}
				else throw ": attendu !";
				if (IsKeyWord(":")) flag = true;
				LireLexeme ();
			}
			Send ("GOLONG:" + l2);
			Send ("LABEL:" + l1);			
			CString tmp1 = LabelBreak, tmp2 = LabelContinue;
			LabelBreak = l3; LabelContinue = l2;
			CompileInstrs ();
			LabelBreak = tmp1; LabelContinue = tmp2;
			if (!IsKeyWord(";")) throw "; attendu !";
			LireLexeme ();			
			Send ("LABEL:" + l2);
		}
		if (IsKeyWord("ELSE"))
		{ 
			LireLexeme (); 
			CompileInstrs ();
			if (!IsKeyWord(";")) throw "! attendu !";
			LireLexeme ();
		}
		Send ("LABEL:" + l3);
		if (typ!="CHAR") Send ("DROP:" + typ); else Send ("DROP:BYTE");
		if (!IsKeyWord("END")) throw "END attendu !";		
		LireLexeme ();
		return;
	}
	
	if (IsKeyWord("BREAK"))
	{
		if (LabelBreak == "") throw "Vous n'�tes pas dans un bloc WHILE/REPEAT/CASE/FOR !";
		Send ("GOLONG:"+LabelBreak);
		LireLexeme ();
		return;
	}

	if (IsKeyWord("CONTINUE"))
	{
		if (LabelContinue == "") throw "Vous n'�tes pas dans un bloc WHILE/REPEAT/CASE/FOR !";
		Send ("GOLONG:"+LabelContinue);
		LireLexeme ();
		return;
	}

	if (IsKeyWord("EXIT"))
	{
		if (LabelBreak == "")
			Send ("HALT");
		else			
			Send ("GOLONG:"+LabelBreak);
		LireLexeme ();
		return;
	}

	if (IsKeyWord("HALT"))
	{
		Send ("HALT");
		LireLexeme ();
		return;
	}

	if (IsKeyWord("RESULT"))
	{
		if (ProcName=="") throw "Ce mot cl� est r�serv� � une utilisation dans les fonctions !";
		lexeme.Type = ID;
		lexeme.Valeur = ProcName;
	}

	if (lexeme.Type==ID)
	{
		if (IsVariable(lexeme.Valeur))
		{
			if (NomVariablesConstantes.Find(lexeme.Valeur,localCste)!=NULL)
				throw "Ne peut pas modifier les constantes !";
			
			if ((NomVariablesConstantes.Find(lexeme.Valeur)!=NULL)
				&& (!IsVariable(lexeme.Valeur, &VariablesLocales))
				&& (!IsVariable(lexeme.Valeur, &VariablesStatiques, localStatique)))
				throw "Ne peut pas modifier les constantes !";

			CString Type1 = AccesVariable ();
			
			LireLexeme ();
			if (!IsKeyWord(":=")) throw ":= attendu !";
			
			CString Type2 = Eval();
			
			CompileConversion (Type2, Type1);
			
			if (Type1!="CHAR")
				Send("SAVE:"+Type1);
			else
				Send("SAVE:BYTE");

			return;
		}

		if (IsProcFun(lexeme.Valeur))
		{
			CompileAppelProcedure ();
			return;
		}
		
		throw "Identificateur non d�clar� !";		
	}

	throw "Instruction attendue !";
}


// ---------------------------------
// Appel � une proc�dure ou fonction
//
// Pr� requis : la fonction appel�e (dans lexeme.valeur) existe
//
// Compile l'appel d'une proc�dure : CompileAppelProcedure () -> void
// Compile l'appel � une fonction : CompileAppelFonction () -> Type
//
// ------------------------------

void CCompiler::CompileAppelProcedure ()
{
	CompileAppelFonction (false);
}


// Implantation de l'appel � une proc�dure/fonction
// ------------------------------------------------


CString CCompiler::CompileAppelFonction (bool isFonction)
{	
	// Initialise la liste des proc�dures/fonctions correspondantes � 'nom'
	CList <TProcFun, TProcFun&> PossiblesFonctions;
	
	PossiblesFonctions.RemoveAll();
	POSITION position = ProceduresEtFonctions.GetHeadPosition();
	while (true)	
	{
		TProcFun current = ProceduresEtFonctions.GetAt (position);
		if (current.Nom == lexeme.Valeur) 
		{
			if ((current.Resultat.Nom=="NIL") && (!isFonction)) PossiblesFonctions.AddTail(current);
			if ((current.Resultat.Nom!="NIL") && (isFonction)) PossiblesFonctions.AddTail(current);
		}
		if (position==ProceduresEtFonctions.GetTailPosition()) break;
		ProceduresEtFonctions.GetNext (position);
	}
	
	if (ProceduresEtFonctions.IsEmpty()) throw "Vous confondez proc�dure et fonction";
	
	// Si ce sont des fonctions, on r�serve de la place pour le r�sultat
	if (isFonction)
	{
		TProcFun current = PossiblesFonctions.GetHead();
		if (current.Resultat.Nom!="CHAR") 
			Send ("CST:"+current.Resultat.Nom+":?");
		else
			Send ("CST:BYTE:?");	
	}
	
	// On passe les param�tre et s�lectionne en m�me temps la bonne proc�dure/fonction
	int count=0; TProcFun current;
	
	LireLexeme();
	if (IsKeyWord("("))
	{
		while (true)
		{
			CString typ = "UNKNOWN";

			// Test s'il s'agit uniquement d'un nom de variable
			{
				POSITION ttmp = IndexSource;
				bool tmp2 = EnableWriting;
				EnableWriting = false;
				LireLexeme();
				if (IsVariable(lexeme.Valeur)) typ = AccesVariable();
				LireLexeme ();
				if (!((IsKeyWord(")")) || (IsKeyWord(",")))) typ = "UNKNOWN";
				IndexSource = ttmp;
				EnableWriting = tmp2;
			}

			// Charge soit la valeur d'une expression, soit l'adresse d'un variable
			bool reference;

			if (typ!="UNKNOWN") { LireLexeme(); typ=AccesVariable(); LireLexeme (); reference = true; }
			else { typ = Eval(); reference = false; }

			// S�lection de la proc�dure par �limination
			{
				CString Type1 = typ;			// type du param�tre
				CString Type2 = "AUCUN";		// type compatible avec le param�tre attendu par une fonction
				CString TypeTrouve = "AUCUN";	// type attendu par une fonction (compatible ou non)
				bool indirect;					// Passage par r�f�rence ?

				POSITION tmp = PossiblesFonctions.GetHeadPosition();
				while (true)
				{
					current = PossiblesFonctions.GetAt (tmp);

					position=current.PremiereVariable;
					for (int i=1; (i<count) && (position!=current.DerniereVariable); i++) VariablesParametres.GetNext(position);
					if (((position==current.DerniereVariable) && (count!=0)) || (position==NULL)) { PossiblesFonctions.RemoveAt (tmp); tmp=NULL; }
					else
					{
						if (count!=0) VariablesParametres.GetNext(position);

						if (TypeTrouve!=Type1)
						{
							TVariable var = VariablesParametres.GetAt(position);
							TypeTrouve = var.Type.Nom;
						}

						if (Type2=="AUCUN")
						{
							TVariable var = VariablesParametres.GetAt(position);

							if ((((Type1 == "BYTE") && ((var.Type.Nom == "INTEGER") || (var.Type.Nom == "REAL")))
								 || ((Type1 == "INTEGER") && ((var.Type.Nom == "BYTE") || (var.Type.Nom == "REAL")))
								 || ((Type1 == "CHAR") && (var.Type.Nom == "STRING"))
								 || (Type1 == var.Type.Nom)) && (!var.Type.Indirect))
							{ Type2 = var.Type.Nom;	indirect = var.Type.Indirect; }

							if ((Type1 == var.Type.Nom) && (var.Type.Indirect) && (reference))
							{ Type2 = var.Type.Nom;	indirect = var.Type.Indirect; }
						}

						TVariable var = VariablesParametres.GetAt(position);
						if ((Type2 != var.Type.Nom) || (indirect != var.Type.Indirect)) { PossiblesFonctions.RemoveAt (tmp); tmp=NULL; }
					}

					if (tmp==NULL) tmp=PossiblesFonctions.GetHeadPosition(); else PossiblesFonctions.GetNext (tmp);
					if (tmp==NULL) break;
				}
				if (Type2=="AUCUN") { if (Type1!=TypeTrouve) throw "Type "+Type1+" rencontr� mais "+TypeTrouve+" attendu !";
									  else throw "Seule une variable peut �tre pass�e par r�f�rence !"; }
				if ((reference) && (!indirect))	{ if (Type1!="CHAR") Send ("LOAD:"+Type1); else Send ("LOAD:BYTE");	}
				CompileConversion (Type1, Type2);
			}
			// Fin de la s�lection

			count++;
			if (!IsKeyWord(",")) break;
		}
		if (!IsKeyWord(")")) throw ") attendu !";
		LireLexeme ();
	}

	// S�lection d'une proc�dure qui prend le bon nombre de param�tres
	POSITION tmp = PossiblesFonctions.GetHeadPosition();
	while (true)
	{
		current=PossiblesFonctions.GetNext(tmp);
		position=current.PremiereVariable;
		if (count==0) { if (position==NULL) break; } 
		else
		{
			for (int i=1; i<count; i++) VariablesParametres.GetNext(position);
			if (position==current.DerniereVariable) break;
		}

		if (tmp==NULL) throw "param�tre attendu !";
	}
	
	// Appel de la proc�dure ou de la fonction
	Send ("CALL:"+current.Nom+"@"+IntToStr(current.version)+":"+IntToStr(count));
	
	// Retourne le type renvoy� par la proc�dure ou la fonction
	return (current.Resultat.Nom);
}

// Compile la directive "USES"
// ---------------------------

void CCompiler::CompileUses()
{
	LireLexeme ();
	
	while (true)
	{
		if (ListeUses.Find(lexeme.Valeur)==NULL)
		{
			ListeUses.AddTail(lexeme.Valeur);
			
			// Chargement du fichier
			CSyntaxe* tmp1 = FichierEntree;
			dlg->EtapeFichier (lexeme.Valeur+".UNT");

			if (dlg->calculatrice==HP49) FichierEntree = new CSyntaxe (dlg->directoryUnit(), lexeme.Valeur+".UNT", dlg, lexeme.Valeur+"_49.UNT");
				else FichierEntree = new CSyntaxe (dlg->directoryUnit(), lexeme.Valeur+".UNT", dlg);

			POSITION tmp2 = IndexSource;
			IndexSource = FichierEntree->lexemes.GetHeadPosition();
			if (FichierEntree->Erreur.Fichier=="") 
			{
				Erreur.Message = "Ne peut charger "+lexeme.Valeur+".UNT !";
				Erreur.Fichier = tmp1->Erreur.Fichier;
			} 
			else
				Erreur = FichierEntree->Erreur;
			if (Erreur.Message=="")
			{
				// Construit les lexemes
				try
				{
					CompileUnit ();
				}
				catch(char* err)
				{
					Erreur.Message = err;					
				}
				catch(CString err)
				{
					Erreur.Message = err;
				}				
			}
			delete(FichierEntree);
			FichierEntree = tmp1;
			IndexSource = tmp2;
			if (Erreur.Message!="") throw Erreur.Message;
			Erreur = FichierEntree->Erreur;
		}		
		LireLexeme ();

		// On efface les variables globales	
		if (!VariablesGlobales.IsEmpty())
		{
			POSITION position = VariablesGlobales.GetHeadPosition();
			while (position!=NULL)
			{			
				TVariable tmp = VariablesGlobales.GetAt(position);
				tmp.Nom="@";
				VariablesGlobales.SetAt(position, tmp);
				VariablesGlobales.GetNext(position);
			}
		}
		if (!VariablesStatiques.IsEmpty())
		{
			POSITION position = VariablesStatiques.GetHeadPosition();
			while (position!=NULL)
			{			
				TVariable tmp = VariablesStatiques.GetAt(position);
				tmp.Nom="@";
				VariablesStatiques.SetAt(position, tmp);
				VariablesStatiques.GetNext(position);
			}
		}
		NomVariablesConstantes.RemoveAll();
		
		// Prochaine unit�
		if (!IsKeyWord(",")) break;
		LireLexeme ();			
	}

	dlg->EtapeMessage (statusCompileSyntaxe);
	dlg->EtapeFichier ("");
	dlg->Update ();
	
	if (!IsKeyWord(";")) throw "; attendu!";
	LireLexeme();
}

// Compilation du programme
// ------------------------

void CCompiler::Compile ()
{
	dlg->EtapeMessage (statusCompileSyntaxe);
	dlg->EtapeFichier ("");
	dlg->Update ();

	LireLexeme ();

	// Compile la directive PROGRAM

	if (IsKeyWord("PROGRAM"))
	{
		LireLexeme ();
		if (lexeme.Type != ID) throw "Identificateur attendu !";
		LireLexeme ();
		if (!IsKeyWord(";")) throw "; attendu !";
		LireLexeme ();
	}

	// Compile les variables, fonctions, proc�dures

	bool done = false;

	while (!IsKeyWord("BEGIN"))
	{
		// Compile la directive USES
		if (IsKeyWord("USES"))
		{
			CompileUses ();			
			continue;
		}

		if (!done) { Send ("REM:Fichier "+FichierEntree->Erreur.Fichier); done = true; }

		// Compile la directive RESSOURCE
		if (IsKeyWord("RESSOURCE"))
		{
			CompileRessource ();
			continue;
		}

		// Compile la directive VAR
		if (IsKeyWord("VAR"))
		{
			CompileVar(&VariablesGlobales);
			continue;
		}

		// Compile la directive STATIC
		if (IsKeyWord("STATIC"))
		{
			CompileVar(&VariablesStatiques);
			continue;
		}

		// Compile la directive CONST
		if (IsKeyWord("CONST"))
		{
			CompileVar(&VariablesStatiques, false, true);
			continue;
		}

		// Compile les proc�dures et fonctions
		if (IsKeyWord("PROCEDURE") || IsKeyWord("FUNCTION"))
		{
			CompileProcFun();
			continue;
		}

		throw "BEGIN attendu !";
	}

	// Compile le programme
	Send ("MAIN");
	if (IsKeyWord("BEGIN")) CompileInstrs (); else throw "BEGIN attendu !";

	// Fin du fichier
	if (!IsKeyWord(".")) throw "END. attendu !";
	Send ("END");
}

// Compilation d'une ressource
// ---------------------------

void CCompiler::CompileRessource ()
{
	// Acquisition du nom de la ressource

	LireLexeme ();
	CString filename = lexeme.Valeur+".RES";

	CLdjTextFile fichier(dlg->directoryResource(), filename);

	TRY
	{
		fichier.Open();
	}
	CATCH (CFileException, e)
	{
		throw "Ressource "+lexeme.Valeur+".RES introuvable !";
		return;
	}
	END_CATCH

	// R�cup�ration des constantes

	CList <CString, CString&> Noms;
	CList <int, int&> Adresses;
	Noms.RemoveAll();
	Adresses.RemoveAll();

	int adr=0;
	if (!VariablesStatiques.IsEmpty()) 
		adr = VariablesStatiques.GetTail().Adresse+VariablesStatiques.GetTail().Type.Taille();

	while (true)
	{
		char taillenom = fichier.Read(); if (taillenom==0) break;
		CString nom =""; for (int i=1; i<=taillenom; i++) nom+=fichier.Read();
		fichier.Seek(16-taillenom);
		int largeur = fichier.Read(); if (largeur==-128) largeur=128;
		char hauteur = fichier.Read();
		fichier.Seek(149);
		nom.MakeUpper();
		Noms.AddTail(nom);
		Adresses.AddTail(adr);
		adr+=largeur*hauteur/4+4;
	}

	// Place les donn�es dans le fichier de sortie

	Send ("CONST:RESSOURCE");
	Send ("DATA:"+filename);	
	Send ("ENDCONST");

	// D�finit les constantes

	Send ("CONST:INTEGER");
	while (true)
	{
		if (Noms.IsEmpty()) break;
		CString nom = Noms.RemoveHead();
		Send ("DATA:"+IntToStr(Adresses.RemoveHead()));
		TType tmpT;
		tmpT.Nom="INTEGER";
		tmpT.Tableau=false;
		tmpT.Indirect=false;
		TVariable tmp;
		tmp.Adresse=adr;
		tmp.Nom=nom;
		tmp.Type=tmpT;		
		VariablesStatiques.AddTail(tmp);
		NomVariablesConstantes.AddTail(nom);
		adr+=5;		
	}
	Send ("ENDCONST");

	// Fin du traitement

	LireLexeme();
	if (!IsKeyWord(";")) throw "; attendu !";
	LireLexeme();
}

// Compilation d'une unit�
// -----------------------

void CCompiler::CompileUnit ()
{
	dlg->EtapeMessage (statusCompileSyntaxe);
	dlg->EtapeFichier ("");
	dlg->Update();

	LireLexeme ();

	// Compile la directive UNIT

	if (IsKeyWord("UNIT"))
	{
		LireLexeme ();
		if (lexeme.Type != ID) throw "Identificateur attendu !";
		Send ("UNIT:"+lexeme.Valeur);		
		LireLexeme ();
		if (!IsKeyWord(";")) throw "; attendu !";
		LireLexeme ();
	}

	// Compile les variables, fonctions, proc�dures

	bool done = false;

	while (!(lexeme.Type==END_OF_FILE))
	{
		// Compile la directive USES
		if (IsKeyWord("USES"))
		{
			CompileUses ();
			continue;
		}

		if (!done) { Send ("REM:Fichier "+FichierEntree->Erreur.Fichier); done = true; }

		// Compile la directive VAR
		if (IsKeyWord("VAR"))
		{
			CompileVar(&VariablesGlobales);
			continue;
		}

		// Compile la directive STATIC
		if (IsKeyWord("STATIC"))
		{
			CompileVar(&VariablesStatiques);
			continue;
		}

		// Compile la directive CONST
		if (IsKeyWord("CONST"))
		{
			CompileVar(&VariablesStatiques, false, true);
			continue;
		}

		// Compile les proc�dures et fonctions
		if (IsKeyWord("PROCEDURE") || IsKeyWord("FUNCTION"))
		{
			CompileProcFun();
			continue;
		}

		throw "PROCEDURE/FUNCTION attendue !";
	}
	
	Send ("ENDUNIT");
}
