"""
game of go on a triangular 3-point board  rbh 2024
"""

from trigo_utils import Cell, Color, IO, Board, Move
from time import time
from os import system

def change_string(p, where, ch):
  return p[:where] + ch + p[where+1:]

class Game_state: 
  def __init__(self):
    self.board = Board.empty()
    bcopy = self.board[:]          # copy of initial board
    self.board_history  = [bcopy]  # history of boards
    self.move_history = [Move.nil] # history of moves
    self.moves_made = 0

  def show_history(self):
    nb = len(self.board_history)
    nm = len(self.move_history)
    assert(nb == nm)
    assert(nb == 1 + self.moves_made)
    print(Color.grn('\n history'),
          Color.mgn(' move cell board'))
    for j in range(nb):
       print(8*' ', '{:4} {:4}   {:3}'.format(j, 
         self.move_history[j], self.board_history[j]))

  def show_kids(self):
    brd = self.board
    print('b kids', Board.children(brd,Cell.b))
    print('w kids', Board.children(brd,Cell.w))

  def make_move(self, new_psn, where):
     if where != Move.p and new_psn in self.board_history:
       self.fail_msg('superko violation')
       return
     self.board = new_psn
     self.board_history.append(new_psn)
     self.move_history.append(where)
     self.moves_made += 1

  def undo_move(self):
    if self.moves_made == 0:
      self.fail_msg('nothing to undo')
      return
    self.board_history.pop()
    self.board = self.board_history[-1]
    self.move_history.pop()
    self.moves_made -= 1

  def fail_msg(self, msg):
    print(Color.mgn('\n sorry,'), end=' ')
    print(msg)

  def wrong_player_msg(self, parity):
    print(Color.mgn('\n sorry,'), end=' ')
    print('not', ("black's","white's")[1-parity], 'turn')

  def request_move(self, cmd):
    cmd = cmd.split()
    if len(cmd)==2:
      color = cmd[0][0]
      if color == 'b': color = Cell.b
      if color == 'w': color = Cell.w
      if color == Cell.e:
        self.fail_msg('color must be black or white')
        return
      parity = self.moves_made % 2
      whose_turn = (Cell.b, Cell.w)[parity]
      if color != whose_turn:
        self.wrong_player_msg(parity)
        return
      if color in Cell.io_ch:
        q = cmd[1][0]
        if q == 'p':
          same_psn = self.board[:] # new copy
          self.make_move(same_psn, Move.p)
        if q.isdigit():
          q = int(q)
          if q >= 3:
            self.fail_msg('cells 0, 1, 2 only')
            return 
          else:
            where = q
            if self.board[where] != Cell.e:
              self.fail_msg('cell occupied')
              return 
            else:
              new_psn = Board.change_cell(self.board, color, where) 
              p_count = new_psn.count(color)
              if p_count == 3:
                self.fail_msg('that move is suicide')
                return
              o_color = Cell.opponent(color)
              o_count = new_psn.count(o_color)
              if p_count + o_count == 3:
                #print('capture')
                new_psn = Board.clear_color(new_psn, o_color)
                #print(new_psn)
              self.make_move(new_psn, where)
              return 

  def interact(self):
    gs = self
    print(IO.welcome)
    print(IO.menu)
    while True:
      Board.report(gs.board)
      gs.show_history()
      #gs.show_kids()
      if gs.move_history[-1] == Move.p and \
         gs.move_history[-2] == Move.p:
        print('\n consecutive passes: game over ... adios :)\n')
        return
      cmd = input('  ')
      if len(cmd)==0:
        print('\n ... adios :)\n')
        return
      c0 = cmd[0][0]
      if c0 == 'h':
        print(IO.menu)
      elif c0 == 'u':
        gs.undo_move()
      elif c0 in Cell.io_ch + 'bw':
        gs.request_move(cmd)
      else:
        gs.fail_msg('could not parse request: please try again\n')
        print(IO.menu)

#start_time = time()
#print('\ntime ', time() - start_time)
#Cell.test()
system('clear')
s = Game_state()
#Board.test(s.board)
s.interact()

