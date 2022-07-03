// Syntaxe.cpp: implementation of the CSyntaxe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Syntaxe.h"


/*                 CSyntaxe::CSyntaxe
 *                 ------------------
 *
 * Pré requis      : Néant
 * Effets          : Initialise la liste des lexemes, les compteurs
 *                   de caractères, la liste des positions des débuts 
 *                   de ligne, et l'objet 'erreur'
 *                   Traduit le fichier en lexeme, et rend compte
 *                   des erreurs rencontrées
 * Modifie         : 'FichierEntree', 'Erreur'
 */

CSyntaxe::CSyntaxe(CString repertoire, CString fichier, CHPPASCALCompilerCtrl *dialogue, CString tryFirst)
{
	dlg = dialogue;
	
	// Initialisation dans la boite de dialogue
	
	dlg->EtapeMessage ("Chargement du fichier");	
	dlg->EtapeJauge(0);
	dlg->Update ();

	// Initialisation de la liste des lexèmes

	lexemes.RemoveAll ();

	// Initialise les compteurs de caracères dans le fichier source

	count = 0;
	count_eof = 0;

	// Initialisation de la liste des positions des débuts de ligne

	DebutLignes.RemoveAll();
	DebutLignes.AddTail(count);

	// Initialise l'état des erreurs rencontrées

	Erreur.Message = "";
	Erreur.Ligne = 0;
	Erreur.Abscisse = 0;

	// Chargement du fichier

	TRY
	{
		if (tryFirst!="")
		{
			FichierEntree = new CLdjTextFile(repertoire, tryFirst);
			tryFirst="...";
			FichierEntree->Open();
		}
		else
		{
			FichierEntree = new CLdjTextFile(repertoire, fichier);
			FichierEntree->Open();		
		}
	}
	CATCH (CFileException, e)
	{
		if (tryFirst=="...") tryFirst="Failure!";
		else
		{
			Erreur.Message = "Fichier introuvable !";
			return;
		}
	}
	END_CATCH

	TRY
	{
		if (tryFirst=="Failure!")
		{		
			FichierEntree = new CLdjTextFile(repertoire, fichier);
			FichierEntree->Open();		
		}
	}
	CATCH (CFileException, e)
	{
			Erreur.Message = "Fichier introuvable !";
			return;
	}
	END_CATCH

	Erreur.Fichier = fichier;

	// Construction des lexemes

	tmp.Ligne = 0;
	tmp.Abscisse = 0;	

	try
	{
		Eof = false; Asm_Flag = false;
		while (!Eof) 
		{
			if (!Asm_Flag) LireLexeme();
			else LireLexemeAsm();
		}
	}

	catch(char* err)
	{
		Erreur.Message = err;
		Erreur.Ligne = tmp.Ligne;
		Erreur.Abscisse = tmp.Abscisse;
	}

	dlg->EtapeJauge (100);
}


/*                 CSyntaxe::CSyntaxe
 *                 -------------------
 *
 * Pré requis      : Néant
 * Effets          : Néant
 * Modifie         : Néant
 */

CSyntaxe::~CSyntaxe()
{
}


/*                 Debug
 *                 -----
 *
 * Pré requis      : Néant
 * Effets          : Génère un fichier de sortie
 *                   dans lequel les différents éléments
 *                   syntaxiques sont identifiés de la facon
 *                   suivante :
 *                    "NE:data" nombre entier
 *                    "NB:data" nombre réel
 *                    "CS:data" chaine de caractère
 *                    "CC:data" caractère simple
 *                    "MC:data" mot clés du langage Pascal
 *                    "ID:data" identificateur
 *                    "ASM:data" instruction assembleur
 *                    où 'data' est codé comme chaine de caractère
 *                   (il y a passage à la ligne après chaque élément)
 * Modifie         : Néant
 */

void CSyntaxe::Debug ()
{
	// Création du fichier de sortie de débogage

	CFile FichierSortie ((CString) "Resultat1.txt", CFile::modeCreate | CFile::modeWrite);

	// Ecriture des lexèmes dans ce fichier

	POSITION position = lexemes.GetHeadPosition();
	while (position!=NULL)
	{
		TLexeme lex = lexemes.GetNext(position);
		CString chaine;
		switch (lex.Type)
		{
			case KEYWORD: chaine = "mot clé"; break;
			case ID: chaine = "identificateur"; break;
			case ENTIER: chaine = "nombre entier"; break;
			case FLOTTANT: chaine = "nombre flottant"; break;
			case CARACTERE: chaine = "caractère"; break;
			case CHAINE: chaine = "chaine de caractères"; break;
			case ASSEMBLER: chaine = "instruction assembleur"; break;
			case END_OF_FILE: chaine = "fin de fichier"; break;
		}
		chaine+=" "+lex.Valeur+"\r\n";
		FichierSortie.Write(chaine.GetBuffer(chaine.GetLength()), chaine.GetLength());
	}

	// Cloture du fichier de sortie

	FichierSortie.Close();
}


/*                 Abscisse
 *                 --------
 *
 * Pré requis      : La position 'pos' doit être valide et la liste
 *                   'DebutLignes' doit contenir les positions des 
 *                   débuts de lignes pour celles précédant la ligne
 *                   dans laquelle figure la position 'pos'
 * Effets          : Renvoie l'abscisse correspondant à 'pos'
 *                   dans le fichier source
 * Modifie         : Néant
 */

int CSyntaxe::Abscisse (int pos)
{
	// Détermine la position du début de la ligne correspondant à 'pos'

	POSITION position = DebutLignes.GetTailPosition ();
	while (pos<DebutLignes.GetAt(position))
		DebutLignes.GetPrev(position);

	// Renvoie la différence entre 'pos' cette position

	return (pos-DebutLignes.GetAt(position));
}


/*                 Ordonnee
 *                 --------
 *
 * Pré requis      : La position 'pos' doit être valide et la liste
 *                   'DebutLignes' doit contenir les positions des 
 *                   débuts des lignes précédant celle décrite par 'pos'
 *                   et classées dans l'ordre croissant
 * Effets          : Renvoie le numéro de ligne correspondant à 'pos'
 * Modifie         : Néant
 */

int CSyntaxe::Ordonnee (int pos)
{
	// Détermine la position du début de ligne correspondant à 'pos'

	POSITION position = DebutLignes.GetTailPosition ();
	while (pos<DebutLignes.GetAt(position))
		DebutLignes.GetPrev(position);

	// Compte le nombre d'éléments de 'DebutLignes' précédant 'position'

	int res = 0;
	while (position!=NULL)
	{
		res++;
		DebutLignes.GetPrev(position);
	}

	return (res);
}


/*                 LireOctet
 *                 ---------
 *
 * Pré requis      : 'FichierEntree' doit être initialisé
 * Effets          : Renvoie le prochain caractère lu dans le 
 *                   fichier source
 * Modifie         : La valeur de Eof si la fin de fichier est atteinte
 *                   Le compteur 'count_eof' au cas où des lectures 
 *                   multiples seraient requises en fin de fichier
 *                   Met à jour un compteur 'count' pointant sur le fichier
 *                   source et met à jour la liste 'DebutLignes' lorsqu'on 
 *                   rencontre un retour charriot, pour y placer la position 
 *                   correspondant au début de la nouvelle ligne.
 */

char CSyntaxe::LireOctet ()
{
	// Renvoie l'octet suivant et met à jour 'Eof' et 'count_eof'

	char octet = 0;
	if (FichierEntree->eof()) { Eof = true; count_eof ++; }
	else octet=FichierEntree->Read();

	// Met à jour le compteur count

	count++;
	if (count%25==0) 
	{
		dlg->EtapeJauge(count*100/FichierEntree->GetLength());
		dlg->Update ();
	}

	// Met à jour la liste 'DebutLignes'

	if (octet=='\n') DebutLignes.AddTail(count);

	// Renvoie l'octet lu

	return octet;
}


/*                 RendreOctet
 *                 -----------
 *
 * Pré requis      : 'FichierEntree' doit être initialisé, ainsi que
 *                   'count' et 'count_eof'
 * Effets          : Recule d'un caractère dans la lecture du fichier
 *                   source
 * Modifie         : Le positionnement dans le fichier source et les valeurs
 *                   de 'count' et 'count_eof'
 */

void CSyntaxe::RendreOctet ()
{
	if (count_eof == 0)
	{
		FichierEntree->Seek(-1);
	}
	else 
		count_eof--;
	count--;
}


// Lecture d'un nombre entier
// --------------------------

void CSyntaxe::LireNombreEntier (CString accumulateur)
{
	char octet = LireOctet();

	if ((octet>='0')  && (octet<='9')) LireNombreEntier (accumulateur+octet);
	else 
	{
		if (octet=='.') 
			LireNombreReel (accumulateur);
		else
		{
			RendreOctet();			
			tmp.Type = ENTIER;
			tmp.Valeur = accumulateur;
			lexemes.AddTail (tmp);
		}
	}
}

// Lecture d'un nombre reel
// --------------------------

void CSyntaxe::LireNombreReel (CString accumulateur)
{
	char octet;

	octet = LireOctet ();
	if((octet>='0') && (octet<='9'))
	{
		accumulateur += '.';
		while ((octet>='0') && (octet<='9'))
		{
			accumulateur += octet;
			octet = LireOctet ();
		}
		if ((octet=='E') || (octet=='e'))
		{
			octet = LireOctet ();

			if ((octet=='+') || (octet=='-'))
			{
				char sgn = octet;
				octet = LireOctet ();
				if((octet>='0') && (octet<='9'))
				{
					accumulateur += 'E';
					if (sgn=='-') accumulateur += sgn;
					while ((octet>='0') && (octet<='9'))
					{
						accumulateur += octet;
						octet = LireOctet ();
					}
				}
				else
				{
					RendreOctet ();
					RendreOctet ();
				}				
			}
			else
				if((octet>='0') && (octet<='9'))
				{
					accumulateur += 'E';
					while ((octet>='0') && (octet<='9'))
					{
						accumulateur += octet;
						octet = LireOctet ();
					}
				}
				else
					RendreOctet ();
		}
		RendreOctet();		
		tmp.Type = FLOTTANT;
		tmp.Valeur = accumulateur;		
		lexemes.AddTail (tmp);
	}
	else
	{
		RendreOctet();
		RendreOctet();
		tmp.Type = ENTIER;
		tmp.Valeur = accumulateur;
		lexemes.AddTail (tmp);	
	}
}

// Lecture d'une chaine de caractère (ou d'un caractère isolé)
// -----------------------------------------------------------

void CSyntaxe::LireChaine (CString accumulateur)
{
	char octet = LireOctet ();

	if (accumulateur.GetLength()==128) throw "Chaine trop longue !";

	if (octet!='\'') LireChaine (accumulateur+octet);
	else 
	{
		octet = LireOctet ();
		if (octet=='\'')
			LireChaine (accumulateur+octet);
		else
		{
			RendreOctet();
			tmp.Valeur = accumulateur;		
			if (accumulateur.GetLength()==1)
				tmp.Type=CARACTERE; 
			else 
				tmp.Type=CHAINE;
			lexemes.AddTail (tmp);		
		}
	}
}

// Lecture d'un mot (identificateur ou mot clé)
// --------------------------------------------

CString CSyntaxe::LireMot (CString accumulateur)
{
	char octet = LireOctet();
	
	if (((octet>='A') && (octet<='Z')) ||
		((octet>='a') && (octet<='z')) ||
		((octet>='0') && (octet<='9')) ||
		(octet=='_'))
	{
		if (accumulateur.GetLength() < 200)
			return (LireMot (accumulateur+octet));
		else throw "Chaine trop longue !";
	}
	else
	{
		RendreOctet();
		return (accumulateur);
	}
}

// Lecture d'un symbole
// --------------------

CString CSyntaxe::LireSymbole (char tampon)
{
	char octet = LireOctet ();

	if (((tampon == '.') && (octet == '.')) ||
		((tampon == ':') && (octet == '=')) ||
		((tampon == '<') && (octet == '=')) ||
		((tampon == '>') && (octet == '=')) ||
		((tampon == '=') && (octet == '<')) ||
		((tampon == '<') && (octet == '>')) ||
		((tampon == '=') && (octet == '>')))		
		return ((CString) tampon + octet);	
	else
	{
		RendreOctet ();
		return ((CString) tampon);
	}
}

// Lecture d'un commentaire
// ------------------------

void CSyntaxe::LireCommentaire ()
{
	char octet = LireOctet ();
	
	if ((octet != '}') && (octet != 0)) LireCommentaire();
}

// Classement d'un mot comme identificateur ou mot clé
// ---------------------------------------------------

void CSyntaxe::McOuId (CString mot)
{
	mot.MakeUpper();	

	if ((mot=="FALSE") || (mot=="TRUE") || (mot=="NOT") || (mot=="RESULT") ||
		(mot=="AND") || (mot=="OR") || (mot=="IF") || (mot=="THEN") || 
		(mot=="ELSE") ||(mot=="CASE") || (mot=="WHILE") || (mot=="DO") || (mot=="REPEAT") ||
		(mot=="TO") || (mot=="DOWNTO") || (mot=="CHR") || (mot=="ORD") ||
		(mot=="ROUND") || (mot=="DIV") || (mot=="MOD") || (mot=="STATIC") ||
		(mot=="CONST") || (mot=="EXIT") || (mot=="BREAK") || (mot=="CONTINUE") ||
		(mot=="HALT") || (mot=="UNTIL") || (mot=="COPY") || (mot=="LENGTH") ||
		(mot=="REALTOSTR") || (mot=="STRTOREAL") || (mot=="INTTOSTR") || (mot=="STRTOINT") || (mot=="IN") ||
		(mot=="FOR") || (mot=="BEGIN") || (mot=="END") || (mot=="CHAR") ||
		(mot=="STRING") || (mot=="INTEGER") || (mot=="BOOLEAN") ||  (mot=="REAL") ||
		(mot=="ARRAY") || (mot=="OF") || (mot=="VAR") || (mot=="BYTE") ||
		(mot=="PROCEDURE") || (mot=="FUNCTION") || (mot=="PROGRAM") || (mot=="UNIT") ||
		(mot=="RESSOURCE") ||(mot=="USES") || (mot=="ASM"))
		tmp.Type=KEYWORD; else tmp.Type=ID;

	tmp.Valeur=mot;

	if (mot=="ASM") Asm_Flag = true; else lexemes.AddTail (tmp);
//	if (mot=="HP49") HP49 = true;
}

// Classement d'un symbole comme mot clé ou erreur de syntaxe
// ----------------------------------------------------------

void CSyntaxe::McOuErreur (CString mot)
{
	if ((mot=="(") || (mot==",") || (mot=="[") || 
		(mot=="=") || (mot=="<") || (mot==">") ||
		(mot=="<=") || (mot==">=") || (mot=="<>") ||
		(mot==">=") || (mot=="=<") || (mot=="=>") ||
		(mot=="..") || (mot==":="))
	{
		tmp.Type=KEYWORD;
		tmp.Valeur=mot;
		lexemes.AddTail (tmp);
		char octet = LireOctet();
		if (octet=='-') 
		{
			octet = LireOctet ();
			if (((octet>='0') && (octet<='9')) || (octet=='.'))
			{
				RendreOctet();
				LireNombreEntier ("-");
				return;
			}
			else
			{
				tmp.Type=KEYWORD;
				tmp.Valeur="-";
				lexemes.AddTail (tmp);
				RendreOctet();
				return;
			}
		}
		else
			RendreOctet();
		return;
	}

	if ((mot=="(") || (mot==",") || (mot==")") ||
		(mot=="[") || (mot=="]") || (mot=="*") ||
		(mot=="/") || (mot=="-") || (mot=="+") ||
		(mot=="=") || (mot=="<") || (mot==">") ||
		(mot==";") || (mot==".") || (mot==":") ||
		(mot=="<=") || (mot==">=") || (mot=="<>") ||
		(mot==">=") || (mot=="=<") || (mot=="=>") ||
		(mot=="..") || (mot==":="))
	{
		tmp.Type=KEYWORD;
		tmp.Valeur=mot;
		lexemes.AddTail (tmp);
	}
	else throw "Symbole inconnu !";
}

// Lecture du prochain lexeme
// --------------------------

void CSyntaxe::LireLexeme ()
{
	char octet = LireOctet();

	switch(octet)
	{

	case 0 : return; break; // Fin du fichier

	case ' ' : case '\n' : case '\r' : case '\t' :
		LireLexeme();
		return;
		break;

	case '{' :
		LireCommentaire();
		LireLexeme();
		return;
		break;

	default :
		tmp.Abscisse = Abscisse (count);
		tmp.Ligne = Ordonnee (count);
	}

	switch (octet)
	{
	case '.' :
		octet = LireOctet ();
		if ((octet>='0') && (octet<='9'))
		{
			RendreOctet ();
			LireNombreReel ("0");
			break;
		}
		else
		{
			RendreOctet ();
			McOuErreur(LireSymbole('.'));
			break;
		}

	case '*' : case '+' : case '-' : case '/' : case ';' :
	case '(' : case ')' : case '[' : case ']' : case ':' :
	case '<' : case '>' : case '=' : case ',' :
		McOuErreur(LireSymbole(octet));
		break;

	case '0' : case '1' : case '2' : case '3' : case '4' :
	case '5' : case '6' : case '7' : case '8' : case '9' :		
		LireNombreEntier(octet);
		break;

	case '\'' :		
		LireChaine ("");
		break;
		
	default:		
		if (((octet>='A') && (octet<='Z')) || 
			((octet>='a') && (octet<='z')))			
			McOuId(LireMot(octet));
		else McOuErreur (octet);
	}
}

void CSyntaxe::LireLexemeAsm ()
{
	char octet = LireOctet();
	
	switch(octet) 
	{
	
	case 0 : return; break; // Fin du fichier

	case ' ' : case '\n' : case '\r' : case '\t' :
		LireLexemeAsm();
		return;
		break;
	}

	tmp.Abscisse = Abscisse (count);
	tmp.Ligne = Ordonnee (count);

	CString resultat = "";

	while ((octet!='\r') && (octet!=0))
	{
		resultat += octet;
		resultat.MakeUpper();
		if (resultat == "END") { Asm_Flag = false; return; }
		octet = LireOctet ();
	}

	if (octet==0) throw "END attendu !";

	tmp.Type = ASSEMBLER;
	tmp.Valeur = resultat;
	lexemes.AddTail (tmp);
}
